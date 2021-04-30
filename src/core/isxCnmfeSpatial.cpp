#include "isxCnmfeSpatial.h"
#include "isxCnmfeUtils.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ThreadPool.h"


namespace isx
{
    static void gradient(const MatrixFloat_t & inArr, MatrixFloat_t & outXGrad, MatrixFloat_t & outYGrad)
    {
        outXGrad = arma::zeros<MatrixFloat_t>(arma::size(inArr));
        outYGrad = arma::zeros<MatrixFloat_t>(arma::size(inArr));

        for (size_t rowIdx = 0; rowIdx < inArr.n_rows; ++rowIdx)
        {
            for (size_t colIdx = 0; colIdx < inArr.n_cols; ++colIdx)
            {
                if (rowIdx == 0)
                {
                    outYGrad(rowIdx, colIdx) = inArr(1, colIdx) - inArr(0, colIdx);
                }
                else if (rowIdx == inArr.n_rows - 1)
                {
                    outYGrad(rowIdx, colIdx) = inArr(inArr.n_rows-1, colIdx) - inArr(inArr.n_rows-2, colIdx);
                }
                else
                {
                    outYGrad(rowIdx, colIdx) = (inArr(rowIdx+1, colIdx) - inArr(rowIdx-1, colIdx)) / 2.f;
                }

                if (colIdx == 0)
                {
                    outXGrad(rowIdx, colIdx) = inArr(rowIdx, 1) - inArr(rowIdx, 0);
                }
                else if (colIdx == inArr.n_cols - 1)
                {
                    outXGrad(rowIdx, colIdx) = inArr(rowIdx, inArr.n_cols-1) - inArr(rowIdx, inArr.n_cols-2);
                }
                else
                {
                    outXGrad(rowIdx, colIdx) = (inArr(rowIdx, colIdx+1) - inArr(rowIdx, colIdx-1)) / 2.f;
                }
            }
        }
    }

    void circularConstraint(MatrixFloat_t & img)
    {
        // Get indices matrix of non-zero elements
        arma::umat indices = arma::ind2sub(arma::size(img), arma::find(img));

        // Return if matrix has no non-zero elements
        if (indices.n_cols == 0)
        {
            return;
        }

        arma::uword rmin = indices.row(0).min();
        arma::uword rmax = indices.row(0).max();
        arma::uword cmin = indices.row(1).min();
        arma::uword cmax = indices.row(1).max();

        if ((rmax + 1 - rmin) * (cmax + 1 - cmin) <= 1)
        {
            return;
        }

        // Take roi of image excluding zero borders
        MatrixFloat_t roi = img(arma::span(rmin, rmax), arma::span(cmin, cmax));

        // Get index and value of max val in matrix
        arma::uvec indmax = arma::ind2sub(arma::size(roi), roi.index_max());
        arma::uword y0 = indmax(0);
        arma::uword x0 = indmax(1);
        float maxval = roi(y0, x0);

        MatrixFloat_t xGrad, yGrad;
        gradient(roi, xGrad, yGrad);

        // Compute meshgrids
        MatrixFloat_t xmesh = arma::repmat(
            arma::linspace<RowFloat_t>(0, static_cast<float>(roi.n_cols) - 1, roi.n_cols), roi.n_rows, 1
        );
        MatrixFloat_t ymesh = arma::repmat(
            arma::linspace<ColumnFloat_t>(0, static_cast<float>(roi.n_rows) - 1, roi.n_rows), 1, roi.n_cols
        );

        arma::umat ind = ((xGrad % (static_cast<float>(x0) - xmesh) + yGrad % (static_cast<float>(y0) - ymesh)) < 0)
                            % (roi < maxval / 3);
        roi(arma::find(ind)).zeros();

        // Get binary mask of non-zero values
        arma::Mat<uint8_t> roiMask = arma::conv_to<arma::Mat<uint8_t>>::from(roi > 0);

        // Use OpenCV to get labelled connected components
        cv::Mat cvMat;
        cv::transpose(cv::Mat(static_cast<int>(roiMask.n_cols), static_cast<int>(roiMask.n_rows), CV_8UC1, roiMask.memptr()), cvMat);

        cv::Mat cvLabels;
        cv::connectedComponents(cvMat, cvLabels, 4);

        cv::transpose(cvLabels, cvLabels);
        arma::Mat<int32_t> labels(reinterpret_cast<int32_t*>(cvLabels.data), cvLabels.cols, cvLabels.rows);

        // Zero out everything that doesn't belong to component with max value
        arma::umat isolatedPix = arma::find(labels != labels(y0, x0));
        roi(isolatedPix).zeros();

        // Set roi back to output image
        img(arma::span(rmin, rmax), arma::span(cmin, cmax)) = roi;
    }

    void connectivityConstraint(
        MatrixFloat_t & img,
        const float inThresh,
        const int32_t inFilterSize)
    {
        // Convert to OpenCV Mat
        cv::Mat cvMat;
        cv::transpose(cv::Mat(static_cast<int>(img.n_cols), static_cast<int>(img.n_rows), CV_32FC1, img.memptr()), cvMat);

        // Apply morphological opening
        cv::Mat cvFilter = cv::Mat::ones(inFilterSize, inFilterSize, CV_8UC1);
        cv::Mat cvOpened;
        cv::morphologyEx(cvMat, cvOpened, cv::MORPH_OPEN, cvFilter);

        // Find max pixel val and index
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(cvMat, &minVal, &maxVal, &minLoc, &maxLoc);

        // Apply threshold
        cv::Mat cvTmp = (cvOpened > (maxVal * inThresh));

        // Label components
        cv::Mat cvLabels;
        cv::connectedComponents(cvTmp, cvLabels, 4);

        // Zero out everything that doesn't belong to component with max value
        cvMat.setTo(0, cvLabels != cvLabels.at<int32_t>(maxLoc));

        // Convert back to Armadillo Mat
        cv::transpose(cvMat, cvMat);
        img = arma::Mat<float>(reinterpret_cast<float*>(cvMat.data), cvMat.cols, cvMat.rows);
    }

    void thresholdComponentsParallel(MatrixFloat_t & inOutA, const int32_t inCloseKSize)
    {
        // 1. Apply median filter with 3x3 kernel
        cv::Mat cvMat = armaToCvMat<float, float>(inOutA, false);
        cv::medianBlur(cvMat, cvMat, 3);
        inOutA = cvToArmaMat<float, float>(cvMat, false);

        // 2. Thresholding - remove pixels that don't meet energy threshold
        arma::fvec Asor = arma::sort(arma::vectorise(inOutA), "descend");
        arma::fvec temp = arma::cumsum(arma::square(Asor));

        const float nrgThresh = 0.9999f;
        arma::uvec ff = arma::find(temp < nrgThresh * temp.back());
        arma::uchar_mat bw;
        if (ff.n_elem > 0)
        {
            size_t ind = ff.back();
            inOutA.elem(arma::find(inOutA < Asor(ind))).zeros();
            bw = arma::conv_to<arma::uchar_mat>::from(inOutA >= Asor(ind));
        }
        else
        {
            bw = arma::zeros<arma::uchar_mat>(arma::size(inOutA));
        }

        // 3. Apply morphological closing
        cv::Mat bwCvMat = armaToCvMat<uint8_t>(bw);

        cv::Mat filter = cv::Mat::ones(inCloseKSize, inCloseKSize, CV_8UC1);
        cv::morphologyEx(bwCvMat, bwCvMat, cv::MORPH_CLOSE, filter, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, 0);

        if (*std::max_element(bwCvMat.begin<uint8_t>(), bwCvMat.end<uint8_t>()) == 0)
        {
            inOutA.zeros();
            return;
        }

        // 4. Extract largest connected component
        // Get labelled connected components
        cv::Mat cvLabels;
        cv::connectedComponents(bwCvMat, cvLabels, 8);

        arma::umat labeledArray = cvToArmaMat<int32_t, arma::uword>(cvLabels);
        size_t numFeatures = labeledArray.max();

        // Get energy for each component
        arma::fvec energies(numFeatures);
        for (size_t feature = 0; feature < numFeatures; ++feature)
        {
            energies[feature] = arma::sum(arma::square(inOutA.elem(arma::find(labeledArray == feature + 1))));
        }

        // Find component with highest energy and zero out all others
        arma::uvec sortArgs = arma::sort_index(energies, "descending");
        size_t indm = sortArgs.front();

        inOutA.elem(arma::find(labeledArray != indm + 1)).zeros();
    }

    void thresholdComponents(CubeFloat_t & inOutA, const int32_t inCloseKSize)
    {
        for (size_t idx = 0; idx < inOutA.n_slices; ++idx)
        {
            thresholdComponentsParallel(inOutA.slice(idx), inCloseKSize);
        }
    }

    static void constructDilateParallel(const MatrixFloat_t & inA, arma::umat & outDistInd)
    {
        cv::Mat cvMat = armaToCvMat<float, float>(inA);
        cv::Mat expandCore = (cv::Mat_<uint8_t>(5, 5) <<
            0, 0, 1, 0, 0,
            0, 1, 1, 1, 0,
            1, 1, 1, 1, 1,
            0, 1, 1, 1, 0,
            0, 0, 1, 0, 0
        );
        cv::dilate(cvMat, cvMat, expandCore, cv::Point(-1, -1), 1, cv::BORDER_REFLECT);

        // Output should be a boolean indicator matrix
        outDistInd = cvToArmaMat<float, float>(cvMat) > 0;
    }

    void determineSearchLocation(const CubeFloat_t & inA, arma::ucube & outDistInd)
    {
        outDistInd = arma::ucube(arma::size(inA));
        for (size_t idx = 0; idx < inA.n_slices; ++idx)
        {
            constructDilateParallel(inA.slice(idx), outDistInd.slice(idx));
        }
    }

    void computeIndicator(const CubeFloat_t & inA, std::vector<arma::uvec> & outInd)
    {
        arma::ucube distIndicator;
        determineSearchLocation(inA, distIndicator);
        distIndicator.reshape(distIndicator.n_rows * distIndicator.n_cols, distIndicator.n_slices, 1);
        arma::umat distIndicatorMat = distIndicator.slice(0);   // d x K

        // Get indices of dilated components that show up at each pixel
        for (size_t rowIdx = 0; rowIdx < distIndicatorMat.n_rows; ++rowIdx)
        {
            outInd.emplace_back(arma::find(distIndicator.row(rowIdx)));
        }
    }

    void regressionParallel(
            const CubeFloat_t & inY,
            const MatrixFloat_t & inC,
            const MatrixFloat_t & inNoise,
            const std::vector<arma::uvec> & inIndC,
            const std::pair<size_t, size_t> inPixelRange,
            const ColumnFloat_t & inCct,
            MatrixFloat_t & outA)
    {
        outA = arma::zeros<MatrixFloat_t>(inPixelRange.second - inPixelRange.first, inC.n_rows);

        for (size_t pxIdx = inPixelRange.first; pxIdx < inPixelRange.second; pxIdx++)
        {
            arma::uvec pxCoord = {pxIdx % inY.n_rows, pxIdx / inY.n_rows};
            RowFloat_t y = inY.subcube(arma::span(pxCoord(0)), arma::span(pxCoord(1)), arma::span::all);
            MatrixFloat_t c = inC.rows(inIndC[pxIdx]).t();

            ColumnFloat_t cctTmp;
            if (inIndC[pxIdx].size() > 0)
            {
                arma::uvec tmpInd = arma::find(inIndC[pxIdx] < inCct.size());
                cctTmp = inCct.elem(inIndC[pxIdx].elem(tmpInd));
            }

            if (c.n_cols > 0 && inNoise(pxCoord(0),pxCoord(1)) > 0)
            {
                float lambda = cctTmp.size() > 0 ? 0.5f * inNoise(pxCoord(0),pxCoord(1)) * sqrt(arma::max(cctTmp)) / inC.n_cols : 0.0f;
                ColumnFloat_t beta;
                isx::lassoLars(c, y, beta, lambda, true);

                for (size_t i = 0; i < inIndC[pxIdx].size(); i++)
                {
                    outA(pxIdx-inPixelRange.first, inIndC[pxIdx](i)) = beta(i);
                }
            }
        }
    }

    void updateSpatialComponents(
        const CubeFloat_t & inY,
        CubeFloat_t & inOutA,
        MatrixFloat_t & inOutC,
        const MatrixFloat_t & inNoise,
        const int32_t inCloseKSize,
        const size_t inPixelsPerProcess,
        const size_t inNumThreads)
    {
        std::vector<arma::uvec> ind2;
        computeIndicator(inOutA, ind2);

        // Normalize C
        ColumnFloat_t quotient = (arma::sqrt(arma::sum(arma::square(inOutC), 1)) + std::numeric_limits<float>::epsilon());
        for (size_t colIdx = 0; colIdx < inOutC.n_cols; ++colIdx)
        {
            inOutC.col(colIdx) = inOutC.col(colIdx) / quotient;
        }

        MatrixFloat_t cDot = (inOutC * inOutC.t());
        ColumnFloat_t cct = cDot.diag();

        MatrixFloat_t matA(inOutA.n_rows * inOutA.n_cols, inOutA.n_slices);
        size_t numPixels = inY.n_rows * inY.n_cols;
        if (inNumThreads < 2 || numPixels <= inPixelsPerProcess)
        {
            // Run regression for each pixel sequentially when specified, or when there are fewer pixels than inPixelsPerProcess
            regressionParallel(inY, inOutC, inNoise, ind2, {0, numPixels}, cct, matA);
        }
        else
        {
            // Run regression on pixel batches in parallel
            ThreadPool pool(inNumThreads);

            size_t nBatches =  (numPixels / inPixelsPerProcess) +  (numPixels % inPixelsPerProcess != 0);
            std::vector<MatrixFloat_t> outputAs(nBatches);
            std::vector<std::pair<size_t, size_t>> ranges(nBatches);

            std::vector<std::future<void>> results(nBatches);
            for (size_t idx = 0; idx < nBatches; ++idx)
            {
                ranges[idx].first = idx * inPixelsPerProcess;
                ranges[idx].second = (idx == nBatches - 1) ? numPixels : idx * inPixelsPerProcess + inPixelsPerProcess;

                results[idx] = pool.enqueue(
                    regressionParallel,
                    std::cref(inY),
                    std::cref(inOutC),
                    std::cref(inNoise),
                    std::cref(ind2),
                    std::cref(ranges[idx]),
                    std::cref(cct),
                    std::ref(outputAs[idx])
                );
            }

            size_t cumNumRows = 0;
            for (size_t idx = 0; idx < results.size(); ++idx)
            {
                results[idx].get();
                matA.rows(arma::span(cumNumRows, cumNumRows + outputAs[idx].n_rows - 1)) = outputAs[idx];
                cumNumRows += outputAs[idx].n_rows;
            }
        }

        // Convert matA back to cube
        for (size_t sliceIdx = 0; sliceIdx < inOutA.n_slices; ++sliceIdx)
        {
            inOutA.slice(sliceIdx) = arma::reshape(matA.col(sliceIdx), inOutA.n_rows, inOutA.n_cols);
        }

        thresholdComponents(inOutA, inCloseKSize);
    }
} // namespace isx
