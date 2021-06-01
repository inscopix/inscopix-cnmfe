#include "isxCnmfeInitialization.h"
#include "isxCnmfeSpatial.h"
#include "isxCnmfeNoise.h"
#include "isxCnmfeUtils.h"
#include "isxCnmfeDeconv.h"
#include "isxLog.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <cmath>


namespace isx
{
    void extractAC(
        const CubeFloat_t & inDataFiltered,
        const CubeFloat_t & inDataRaw,
        const std::pair<int32_t,int32_t> centerIndex,
        MatrixFloat_t & outAi,
        ColumnFloat_t & outCi,
        const float minCorrNeuron,
        const float maxCorrBg)
    {
        // normalize data
        MatrixFloat_t tmp (arma::mean(inDataFiltered, 2));
        CubeFloat_t inDataFilteredProcessed = inDataFiltered.each_slice() - tmp;

        tmp = arma::sqrt(arma::sum(arma::square(inDataFilteredProcessed), 2));
        tmp.elem(arma::find(tmp==0.0f)).fill(1.0f);
        inDataFilteredProcessed = inDataFilteredProcessed.each_slice() / tmp;

        // get correlation between trace of each pixel and that at neuron center
        ColumnFloat_t y0 = inDataFilteredProcessed(
            arma::span(centerIndex.first), arma::span(centerIndex.second), arma::span::all
        );
        for (size_t colIndex = 0; colIndex < inDataFilteredProcessed.n_cols; ++colIndex)
        {
            for (size_t rowIndex = 0; rowIndex < inDataFilteredProcessed.n_rows; ++rowIndex)
            {
                ColumnFloat_t vec = inDataFilteredProcessed.subcube(
                    arma::span(rowIndex), arma::span(colIndex), arma::span::all
                );
                tmp(rowIndex, colIndex) = arma::dot(y0, vec);
            }
        }

        // extract activity of neuron
        outCi = arma::zeros<ColumnFloat_t>(inDataFilteredProcessed.n_slices);
        arma::umat neuronIndices = arma::find(tmp > minCorrNeuron);

        if (!neuronIndices.empty())
        {
            for(arma::umat::iterator it = neuronIndices.begin(); it != neuronIndices.end(); ++it)
            {
                arma::uvec u = ind2sub(arma::size(inDataFilteredProcessed), *it);
                outCi += inDataFilteredProcessed(arma::span(u(0)), arma::span(u(1)), arma::span::all);
            }
            outCi /= static_cast<float>(neuronIndices.size());
        }

        // avoid empty results
        if (arma::dot(outCi, outCi) == 0)
        {
            outAi = arma::zeros<MatrixFloat_t>(inDataRaw.n_rows, inDataRaw.n_cols);
            outCi = arma::zeros<ColumnFloat_t>(inDataRaw.n_slices);
            return;
        }

        // extract background activity
        ColumnFloat_t y_bg (inDataRaw.n_slices);
        arma::umat bgIndices = arma::find(tmp < maxCorrBg);
        if (!bgIndices.empty())
        {
            MatrixFloat_t bgColumns(bgIndices.size(), inDataRaw.n_slices);

            for(arma::umat::iterator it = bgIndices.begin(); it != bgIndices.end(); ++it)
            {
                arma::uvec u = arma::ind2sub(arma::size(inDataRaw), *it);
                ColumnFloat_t tmpCol = inDataRaw(arma::span(u(0)), arma::span(u(1)), arma::span::all);
                bgColumns(it - bgIndices.begin(), arma::span::all) = tmpCol.t();
            }

            y_bg = arma::median(bgColumns, 0).t();
        }
        else
        {
            y_bg = arma::ones<ColumnFloat_t>(inDataRaw.n_slices);
        }

        // extract spatial footprint
        MatrixFloat_t X = arma::join_rows(outCi, y_bg);
        X = arma::join_rows(X, arma::ones<ColumnFloat_t>(y_bg.size()));
        MatrixFloat_t XX = X.t() * X;
        MatrixFloat_t Xy (X.n_cols, inDataRaw.n_rows * inDataRaw.n_cols);
        for (size_t colIndex = 0; colIndex < inDataRaw.n_cols; ++colIndex)
        {
            for (size_t rowIndex = 0; rowIndex < inDataRaw.n_rows; ++rowIndex)
            {
                ColumnFloat_t vec = inDataRaw.subcube(arma::span(rowIndex), arma::span(colIndex), arma::span::all);
                Xy(arma::span::all, rowIndex * inDataRaw.n_cols + colIndex) = X.t() * vec;
            }
        }

        outAi = arma::solve(XX, Xy, arma::solve_opts::fast);
        outAi = outAi.row(0);
        outAi.reshape(inDataRaw.n_cols, inDataRaw.n_rows);
        outAi = outAi.t();
        outAi.transform( [](float val) { return (val < 0.0f) ? 0.0f : val; } );

        circularConstraint(outAi);
        connectivityConstraint(outAi);

        // remove baseline from temporal activity
        float sn = getNoiseFft(outCi);

        ColumnFloat_t yDiff(outCi.size());
        yDiff(0) = -1;
        yDiff(arma::span(1, outCi.size() - 1)) = arma::diff(outCi);

        arma::uvec ind = arma::find((yDiff >= 0) && (yDiff < sn));
        if (!ind.empty())
        {
            outCi -= arma::median(outCi.elem(ind));
        }
    }

    static cv::Mat constructDiskFilter(int n)
    {
        cv::Mat spatialFilter = cv::getGaussianKernel(4*n + 1, n, CV_32F);
        cv::Mat spatialFilterTransposed;
        cv::transpose(cv::getGaussianKernel(4*n + 1, n, CV_32F), spatialFilterTransposed);
        spatialFilter = spatialFilter * spatialFilterTransposed;

        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(spatialFilter.row(0), &minVal, &maxVal, &minLoc, &maxLoc);

        cv::Mat mask = spatialFilter >= static_cast<float>(maxVal);
        cv::subtract(spatialFilter, cv::mean(spatialFilter, mask), spatialFilter);
        cv::bitwise_not(mask, mask);
        spatialFilter.setTo(0, mask);
        return spatialFilter;
    }

    static void apply2DFilter(MatrixFloat_t & inMatrix, MatrixFloat_t & outMatrix, cv::Mat & filter)
    {
        cv::Mat tmpMat = armaToCvMat(inMatrix);
        cv::filter2D(tmpMat, tmpMat, -1, filter, cv::Point(-1,-1), 0, 1);
        outMatrix = cvToArmaMat<float>(tmpMat);
    }

    void initNeuronsCorrPNR(
        const CubeFloat_t & inData,
        CubeFloat_t & outA,
        MatrixFloat_t & outC,
        MatrixFloat_t & outCRaw,
        MatrixFloat_t & outS,
        DeconvolutionParams inDeconvParams,
        InitializationParams inInitParams,
        int32_t maxNumNeurons)
    {
        CubeFloat_t inDataModifiable(inData);
        CubeFloat_t inDataProcessed(inData);

        // spatial filtering using disk background filter
        cv::Mat spatialFilter;
        if (inInitParams.m_gaussianKernelSize > 0)
        {
            spatialFilter = constructDiskFilter(inInitParams.m_gaussianKernelSize);

            for (size_t i = 0; i < inDataProcessed.n_slices; ++i)
            {
                MatrixFloat_t inDataSlice (inData.slice(i));
                apply2DFilter(inDataSlice, inDataProcessed.slice(i), spatialFilter);
            }
        }

        // compute PNR image
        MatrixFloat_t tmp (arma::mean(inDataProcessed, 2));
        inDataProcessed.each_slice() -= tmp;

        MatrixFloat_t pixelNoise;
        getNoiseFft(inDataProcessed, pixelNoise, std::pair<float,float>(0.25f, 0.5f), AveragingMethod_t::MEAN);

        tmp = arma::max(inDataProcessed, 2);
        MatrixFloat_t pnr = tmp / pixelNoise;

        // compute local correlation image
        CubeFloat_t tmpCube(inDataProcessed);
        MatrixFloat_t minPixelNoise = static_cast<float>(inInitParams.m_noiseThreshold) * pixelNoise;
        for (size_t i = 0; i < tmpCube.n_slices; ++i)
        {
            arma::umat ind = arma::find(tmpCube.slice(i) < minPixelNoise);
            tmpCube.slice(i).elem(ind).fill(0);
        }

        MatrixFloat_t localCorr;
        computeLocalCorr(tmpCube, localCorr);

        // screen for seed pixels as neuron centers
        MatrixFloat_t vSearch = localCorr % pnr;
        arma::uvec ind = arma::find((localCorr < inInitParams.m_minCorr) || (pnr < inInitParams.m_minPNR));
        vSearch.elem(ind).fill(0);

        // define search space
        arma::umat tmpInd = arma::find(vSearch <= 0);
        MatrixFloat_t indSearch = arma::zeros<MatrixFloat_t>(vSearch.n_rows, vSearch.n_cols);
        indSearch.elem(tmpInd).fill(1);

        // exclude pixels close to boundary from search space
        if (inInitParams.m_boundaryDist > 0)
        {
            for (int i = 0; i < inInitParams.m_boundaryDist; ++i)
            {
                indSearch.row(i).fill(1);
                indSearch.row(vSearch.n_rows - 1 - i).fill(1);
                indSearch.col(i).fill(1);
                indSearch.col(vSearch.n_cols - 1 - i).fill(1);
            }
        }

        // params for neuron initialization loop
        if (maxNumNeurons <= 0)
        {
            maxNumNeurons = static_cast<int32_t>((indSearch.size() - arma::accu(indSearch)) / 5);
        }

        MatrixFloat_t xmesh = arma::repmat(
            arma::linspace<RowFloat_t>(0.0f, static_cast<float>(inData.n_cols - 1), inData.n_cols), inData.n_rows, 1
        );
        MatrixFloat_t ymesh = arma::repmat(
            arma::linspace<ColumnFloat_t>(0.0f, static_cast<float>(inData.n_rows - 1), inData.n_rows), 1, inData.n_cols
        );
        MatrixFloat_t pixel_v = (xmesh*10.0f + ymesh) * 1e-5f;

        int32_t numNeurons = 0;
        bool lookForNeurons = maxNumNeurons > 0;
        float minvSearch = inInitParams.m_minCorr * inInitParams.m_minPNR;

        outA = arma::zeros<CubeFloat_t>(inData.n_rows, inData.n_cols, maxNumNeurons);
        outC = arma::zeros<MatrixFloat_t>(maxNumNeurons, inData.n_slices);
        outCRaw = arma::zeros<MatrixFloat_t>(maxNumNeurons, inData.n_slices);
        outS = arma::zeros<MatrixFloat_t>(maxNumNeurons, inData.n_slices);

        // neuron initialization loop
        while (lookForNeurons)
        {
            tmpInd = arma::find((localCorr < inInitParams.m_minCorr) || (pnr < inInitParams.m_minPNR));
            vSearch.elem(tmpInd).fill(0);

            // convert data to OpenCV format
            cv::Mat vSearchMat = armaToCvMat<float, float>(vSearch);
            cv::Mat pixelvMat = armaToCvMat<float, float>(pixel_v);
            cv::Mat indSearchMat = armaToCvMat<float, uint8_t>(indSearch);

            // apply median blur (only supports CV_8U, CV_16U, CV_32F)
            cv::medianBlur(vSearchMat, vSearchMat, 3);
            vSearchMat += pixelvMat;
            vSearchMat.setTo(0, indSearchMat);

            // select seed pixels as local maximums
            cv::Mat vMax;
            cv::Mat tmpKernel = cv::Mat::ones(static_cast<int32_t>(round(inInitParams.m_averageCellDiameter / 4.0f)),
                                              static_cast<int32_t>(round(inInitParams.m_averageCellDiameter / 4.0f)), CV_32FC1);
            cv::dilate(vSearchMat, vMax, tmpKernel);

            cv::Mat mask;
            cv::bitwise_or(vSearchMat != vMax, vSearchMat < minvSearch, mask);
            vMax.setTo(0, mask);

            // propagate search space updates from opencv matrix to arma matrix
            vSearch = cvToArmaMat<float, float>(vSearchMat);

            // find locations of non-zero pixels
            cv::Mat nonZeroCoordinates;
            cv::findNonZero(vMax > 0, nonZeroCoordinates);

            if (nonZeroCoordinates.empty())
            {
                // no more candidate seed pixels
                break;
            }

            std::vector<cv::Point> ptsVec(nonZeroCoordinates);
            std::sort(ptsVec.begin(), ptsVec.end(),
                  [&vMax](cv::Point const &a, cv::Point const &b)
                  {
                      return vMax.at<float>(a) > vMax.at<float>(b);
                  });

            // loop over candidate seed pixels
            for (auto &it : ptsVec)
            {
                int r = it.y;
                int c = it.x;

                // mark pixel as visited
                indSearch.at(r, c) = 1;

                // skip pixel if it doesn't qualify as a good seed pixel
                if (vSearch.at(r, c) < minvSearch)
                {
                    continue;
                }

                ColumnFloat_t y0 (inDataProcessed(arma::span(r), arma::span(c), arma::span::all));
                y0 = arma::diff(y0);
                if (arma::max(y0) < 3 * arma::stddev(y0, 1))
                {
                    vSearch.at(r, c) = 0;
                    continue;
                }

                // crop small region around seed pixel for estimating spatiotemporal activity of the neuron
                int rMin = std::max(0, r - inInitParams.m_averageCellDiameter);
                int rMax = std::min(static_cast<int>(inData.n_rows), r + inInitParams.m_averageCellDiameter + 1);
                int cMin = std::max(0, c - inInitParams.m_averageCellDiameter);
                int cMax = std::min(static_cast<int>(inData.n_cols), c + inInitParams.m_averageCellDiameter + 1);

                CubeFloat_t dataRawBox(
                    inDataModifiable(arma::span(rMin, rMax - 1), arma::span(cMin, cMax - 1), arma::span::all));
                CubeFloat_t dataFilteredBox(
                    inDataProcessed(arma::span(rMin, rMax - 1), arma::span(cMin, cMax - 1), arma::span::all));

                // extract spatiotemporal activity
                std::pair<int32_t, int32_t> centerIndex(r - rMin, c - cMin);
                MatrixFloat_t ai;
                ColumnFloat_t ciRaw;
                extractAC(dataFilteredBox, dataRawBox, centerIndex, ai, ciRaw);

                arma::uvec nonZeroElems = arma::find(ai > 0);
                if (static_cast<int>(nonZeroElems.size()) < inInitParams.m_minNumPixels)
                {
                    continue;
                }

                outA(arma::span(rMin, rMax - 1), arma::span(cMin, cMax - 1), arma::span(numNeurons)) = ai;
                outCRaw.row(numNeurons) = ciRaw.t();

                ColumnFloat_t ci, si;
                if (inInitParams.m_deconvolve)
                {
                    // with deconvolution
                    float c1, baseline;
                    float noise = -1;
                    std::vector<float> ARParams;
                    constrainedFoopsi(ciRaw, ARParams, noise, ci, baseline, c1, si, inDeconvParams);

                    if (arma::sum(ci) == 0)
                    {
                        continue;
                    }
                    outC.row(numNeurons) = ci.t();
                    outS.row(numNeurons) = si.t();
                }
                else
                {
                    // no deconvolution
                    ci = ciRaw;
                    ci.elem(arma::find(ci < 0)).fill(0);

                    if (arma::sum(ci) == 0)
                    {
                        continue;
                    }
                    outC.row(numNeurons) = ci.t();
                }

                // update search space to exclude nearby pixels
                MatrixFloat_t tmpmat(indSearch(arma::span(rMin, rMax - 1), arma::span(cMin, cMax - 1)));
                tmpmat.elem(find(ai > ai.max() / 2.0f)).fill(1.0f);
                indSearch(arma::span(rMin, rMax - 1), arma::span(cMin, cMax - 1)) = tmpmat;

                // remove spatiotemporal activity of initialized neuron from raw data
                for (size_t i = 0; i < inDataModifiable.n_slices; ++i)
                {
                    inDataModifiable(arma::span(rMin, rMax - 1),
                                     arma::span(cMin, cMax - 1),
                                     arma::span(i)) -= ai * ci.at(i);
                }

                // define neighborhood of pixels to update after initializing a neuron
                int r2Min = std::max(0, r - 2 * inInitParams.m_averageCellDiameter);
                int r2Max = std::min(static_cast<int>(inData.n_rows), r + 2 * inInitParams.m_averageCellDiameter + 1);
                int c2Min = std::max(0, c - 2 * inInitParams.m_averageCellDiameter);
                int c2Max = std::min(static_cast<int>(inData.n_cols), c + 2 * inInitParams.m_averageCellDiameter + 1);

                if (inInitParams.m_gaussianKernelSize > 0) {
                    // spatially filter neuron shape
                    MatrixFloat_t tmpImg(
                        outA(arma::span(r2Min, r2Max - 1), arma::span(c2Min, c2Max - 1), arma::span(numNeurons))
                    );
                    MatrixFloat_t aiFiltered;
                    apply2DFilter(tmpImg, aiFiltered, spatialFilter);

                    // update processed data
                    for (size_t i = 0; i < inDataProcessed.n_slices; ++i)
                    {
                        inDataProcessed(arma::span(r2Min, r2Max - 1), arma::span(c2Min, c2Max - 1),
                                        arma::span(i)) -= aiFiltered * ci.at(i);
                    }
                    dataFilteredBox = inDataProcessed(arma::span(r2Min, r2Max - 1),
                                                      arma::span(c2Min, c2Max - 1), arma::span::all);
                }

                // update PNR image
                MatrixFloat_t maxBox = arma::max(dataFilteredBox, 2);
                MatrixFloat_t noiseBox(
                    pixelNoise(arma::span(r2Min, r2Max - 1), arma::span(c2Min, c2Max - 1)));
                MatrixFloat_t pnrBox(maxBox / noiseBox);
                pnrBox.elem(arma::find(pnrBox < inInitParams.m_minPNR)).fill(0.0f);
                pnr(arma::span(r2Min, r2Max - 1), arma::span(c2Min, c2Max - 1)) = pnrBox;

                // update local correlation image
                // compute local correlation of candidate pixel for size of neuron + 1 pixel border of neighbouring neurons
                int r3Min = std::max(0, r - inInitParams.m_averageCellDiameter - 1);
                int r3Max = std::min(static_cast<int>(inData.n_rows), r + inInitParams.m_averageCellDiameter + 2);
                int c3Min = std::max(0, c - inInitParams.m_averageCellDiameter - 1);
                int c3Max = std::min(static_cast<int>(inData.n_cols), c + inInitParams.m_averageCellDiameter + 2);
                dataFilteredBox = inDataProcessed(arma::span(r3Min, r3Max - 1), arma::span(c3Min, c3Max - 1), arma::span::all);
                noiseBox = minPixelNoise(arma::span(r3Min, r3Max - 1), arma::span(c3Min, c3Max - 1));
                for (size_t i = 0; i < dataFilteredBox.n_slices; ++i)
                {
                    arma::umat tmpind = arma::find(dataFilteredBox.slice(i) < noiseBox);
                    dataFilteredBox.slice(i).elem(tmpind).fill(0);
                }

                MatrixFloat_t cnBox;
                computeLocalCorr(dataFilteredBox, cnBox);
                cnBox.elem(arma::find(cnBox < 0)).fill(0);

                localCorr(arma::span(rMin, rMax - 1), arma::span(cMin, cMax - 1)) = cnBox(
                    arma::span(rMin - r3Min, rMax - r3Min - 1), arma::span(cMin - c3Min, cMax - c3Min - 1));
                cnBox = localCorr(arma::span(r2Min, r2Max - 1), arma::span(c2Min, c2Max - 1));

                // update search space
                vSearch(arma::span(r2Min, r2Max - 1), arma::span(c2Min, c2Max - 1)) = cnBox % pnrBox;
                vSearch.elem(arma::find(indSearch == 1)).fill(0);

                // increment number of neurons initialized
                ++numNeurons;

                if (numNeurons == maxNumNeurons)
                {
                    lookForNeurons = false;
                    break;
                }
            } // for loop for candidate seed pixels
        } // while loop for neurons

        outA = outA.head_slices(numNeurons);
        outCRaw = outCRaw.head_rows(numNeurons);
        outC = outC.head_rows(numNeurons);
        outS = outS.head_rows(numNeurons);

        ISX_LOG_INFO(numNeurons, " neurons were initialized");
    }

} // namespace isx
