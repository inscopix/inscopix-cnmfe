#include "isxCnmfeGreedy.h"
#include "isxCnmfeInitialization.h"
#include "isxCnmfeSpatial.h"
#include "isxCnmfeTemporal.h"
#include "isxCnmfeMerging.h"
#include "isxCnmfeUtils.h"

#include "ThreadPool.h"


namespace isx 
{
    static arma::Mat<uint8_t> generateRing(int32_t radius)
    {
        // construct ring mask
        MatrixFloat_t X = arma::repmat(arma::regspace<RowFloat_t>(static_cast<float>(-radius),  static_cast<float>(radius)), 2*radius + 1, 1);
        arma::Mat<uint8_t> innerDisk = arma::conv_to<arma::Mat<uint8_t>>::from(
            (arma::square(X) + arma::square(X.t())) <= static_cast<float>(radius*radius));

        X = arma::repmat(arma::regspace<RowFloat_t>(static_cast<float>(-(radius+1)),  static_cast<float>(radius+1)), 2*radius + 3, 1);
        arma::Mat<uint8_t> ring = arma::conv_to<arma::Mat<uint8_t>>::from(
            (arma::square(X) + arma::square(X.t())) <= static_cast<float>((radius+1)*(radius+1)));

        ring(arma::span(1,ring.n_rows-2), arma::span(1, ring.n_rows-2)) -= innerDisk;
        return ring;
    }

    static arma::uvec getPixelIndicesOnRing(
        const arma::Mat<uint8_t>& ring, 
        const arma::umat & ringIndices, 
        const int32_t radius,
        const int32_t pixel, 
        const std::pair<int32_t,int32_t> inDims)
    {
        arma::Col<int32_t> x (ringIndices.size());
        arma::Col<int32_t> y (ringIndices.size());
        for (size_t i = 0; i < ringIndices.size(); i++)
        {
            arma::uvec coord = arma::ind2sub(arma::size(ring), ringIndices.at(i));
            x.at(i) = (pixel % inDims.first) + static_cast<int32_t>(coord(0)) - radius - 1;
            y.at(i) = (pixel / inDims.first) + static_cast<int32_t>(coord(1)) - radius - 1;
        }

        arma::uvec inside = arma::find(x >= 0 && x < inDims.first && y >= 0 && y < inDims.second);
        arma::Col<int32_t> indicesOnRing = x.elem(inside) + y.elem(inside) * inDims.first;
        return arma::conv_to<arma::uvec>::from(indicesOnRing);
    }

    static ColumnFloat_t processPixel(
        const int32_t pixel, 
        const arma::uvec & indicesOnRing, 
        const MatrixFloat_t & X)
    {
        MatrixFloat_t B (indicesOnRing.size(), X.n_cols);
        for (size_t i=0; i < X.n_cols; i++)
        {
            ColumnFloat_t tmpCol = X.col(i);
            B.col(i) = tmpCol(indicesOnRing);
        }

        MatrixFloat_t tmp = B * B.t();
        tmp.diag() += arma::sum(tmp.diag()) * 1e-5f;
        ColumnFloat_t res = arma::solve(tmp, B * X.row(pixel).t());
        return res;
    }

    // helper function for processing pixels in parallel for computeW
    static void computeWParallel(
        const arma::Mat<uint8_t> & inRing,
        const arma::umat & inRingIndices,
        const MatrixFloat_t & inX,
        const std::pair<int32_t,int32_t> inDimsSub,
        const int32_t inRadius,
        const int32_t idx,
        arma::uvec & outIndicesOnRing,
        ColumnFloat_t & outValues)
    {
        outIndicesOnRing = getPixelIndicesOnRing(inRing, inRingIndices, inRadius, idx, inDimsSub);
        outValues = processPixel(idx, outIndicesOnRing, inX);
    }

    void computeW(
        const MatrixFloat_t & inY, 
        const MatrixFloat_t & inA, 
        const MatrixFloat_t & inC,
        const std::pair<size_t,size_t> inDims,
        const float inRadius,
        arma::SpMat<float> & outW, 
        ColumnFloat_t & outB0, 
        const size_t spatialSub,
        const size_t inNumThreads)
    {
        int32_t radius = static_cast<int32_t>(std::round(inRadius/static_cast<float>(spatialSub)));
        arma::Mat<uint8_t> ring = generateRing(radius);
        arma::umat ringIndices = arma::find(ring > 0);

        outB0 = arma::mean(inY, 1) - inA * arma::mean(inC, 1);

        MatrixFloat_t X;
        if (spatialSub > 1)
        {
            arma::SpMat<float> decMat = generateDecimationMatrix(inDims, spatialSub);
            X = decMat*inY;
            if (inA.size() > 0)
            {
                X -= decMat * inA * inC;
            }
            X.each_col() -= decMat * outB0;
        }
        else
        {
            X = (inY - inA * inC);
            X.each_col() -= outB0;
        }

        // adjust dimensions based on spatial subsampling factor
        std::pair<int32_t,int32_t> inDimsSub;
        if (spatialSub > 1)
        {
            inDimsSub.first = static_cast<int32_t>((inDims.first-1)/spatialSub + 1);
            inDimsSub.second = static_cast<int32_t>((inDims.second-1)/spatialSub + 1);
        }
        else
        {
            inDimsSub.first = static_cast<int32_t>(inDims.first);
            inDimsSub.second = static_cast<int32_t>(inDims.second);
        }

        // Build COO representation of matrix
        const int32_t numPixels = static_cast<int32_t>(inDimsSub.first * inDimsSub.second);
        size_t numElems = 0;
        size_t maxNumElems = ringIndices.size() * numPixels;
        arma::uvec colIndices(maxNumElems);
        arma::uvec rowIndices(maxNumElems);
        ColumnFloat_t values(maxNumElems);

        if (inNumThreads > 1)
        {
            // Process pixels in parallel
            ThreadPool pool(inNumThreads);
            std::vector<arma::uvec> indicesOnRingVec(numPixels);
            std::vector<ColumnFloat_t> dataVec(numPixels);
            std::vector<std::future<void>> results(indicesOnRingVec.size());

            for (int32_t idx = 0; idx < numPixels; ++idx)
            {
                results[idx] = pool.enqueue(
                    computeWParallel,
                    std::cref(ring),
                    std::cref(ringIndices),
                    std::cref(X),
                    inDimsSub,
                    radius,
                    idx,
                    std::ref(indicesOnRingVec[idx]),
                    std::ref(dataVec[idx])
                );
            }

            for (int32_t idx = 0; idx < numPixels; ++idx)
            {
                results[idx].get();
                rowIndices(arma::span(numElems, numElems + indicesOnRingVec[idx].size() - 1)) = idx * arma::ones<arma::uvec>(indicesOnRingVec[idx].size());
                colIndices(arma::span(numElems, numElems + indicesOnRingVec[idx].size() - 1)) = indicesOnRingVec[idx];
                values(arma::span(numElems, numElems + indicesOnRingVec[idx].size() - 1)) = dataVec[idx].head(indicesOnRingVec[idx].size());
                numElems += indicesOnRingVec[idx].size();
            }
        }
        else
        {
            // Process pixels sequentially
            for (int32_t i = 0; i < numPixels; i++)
            {
                arma::uvec indicesOnRing = getPixelIndicesOnRing(ring, ringIndices, radius, i, inDimsSub);
                ColumnFloat_t data = processPixel(i, indicesOnRing, X);
                rowIndices(arma::span(numElems, numElems + indicesOnRing.size() - 1)) = i * arma::ones<arma::uvec>(indicesOnRing.size());
                colIndices(arma::span(numElems, numElems + indicesOnRing.size() - 1)) = indicesOnRing;
                values(arma::span(numElems, numElems + indicesOnRing.size() - 1)) = data.head(indicesOnRing.size());
                numElems += indicesOnRing.size();
            }
        }

        // Build W from COO data
        arma::umat indices = arma::join_cols(rowIndices.head(numElems).t(), colIndices.head(numElems).t());
        outW = arma::SpMat<float>(indices, values.head(numElems), numPixels, numPixels);
    }

    MatrixFloat_t downscale(const MatrixFloat_t & inY, const std::pair<size_t,size_t> inBlockSize)
    {
        // Check if block size cleanly divides input
        std::pair<size_t, size_t> padding = {0, 0}; // bottom, right
        if (inY.n_rows % inBlockSize.first != 0)
        {
            padding.first = inBlockSize.first - (inY.n_rows % inBlockSize.first);
        }
        if (inY.n_cols % inBlockSize.second != 0)
        {
            padding.second = inBlockSize.second - (inY.n_cols % inBlockSize.second);
        }

        MatrixFloat_t result((inY.n_rows + padding.first) / inBlockSize.first, 
                              (inY.n_cols + padding.second) / inBlockSize.second);
        
        // Skip last row or col if they do not fit in one complete block
        const size_t rowBound = result.n_rows - static_cast<size_t>(padding.first != 0);
        const size_t colBound = result.n_cols - static_cast<size_t>(padding.second != 0);
        for (size_t rowIdx = 0; rowIdx < rowBound; ++rowIdx)
        {
            for (size_t colIdx = 0; colIdx < colBound; ++colIdx)
            {
                MatrixFloat_t block = inY(
                    arma::span(rowIdx * inBlockSize.first, rowIdx * inBlockSize.first + inBlockSize.first - 1),
                    arma::span(colIdx * inBlockSize.second, colIdx * inBlockSize.second + inBlockSize.second - 1)
                );
                result(rowIdx, colIdx) = arma::mean(arma::vectorise(block));
            }
        }

        if (padding.first != 0)
        {
            for (size_t colIdx = 0; colIdx < colBound; ++ colIdx)
            {
                MatrixFloat_t block = inY(
                    arma::span(rowBound * inBlockSize.first, inY.n_rows - 1),
                    arma::span(colIdx * inBlockSize.second, colIdx * inBlockSize.second + inBlockSize.second - 1)
                );
                result(result.n_rows - 1, colIdx) = arma::mean(arma::vectorise(block));
            }
        }

        if (padding.second != 0)
        {
            for (size_t rowIdx = 0; rowIdx < rowBound; ++ rowIdx)
            {
                MatrixFloat_t block = inY(
                    arma::span(rowIdx * inBlockSize.first, rowIdx * inBlockSize.first + inBlockSize.first - 1),
                    arma::span(colBound * inBlockSize.second, inY.n_cols - 1)
                );
                result(rowIdx, result.n_cols - 1) = arma::mean(arma::vectorise(block));
            }
        }

        if (padding.first != 0 && padding.second != 0)
        {
            MatrixFloat_t block = inY(
                arma::span(rowBound * inBlockSize.first, inY.n_rows - 1),
                arma::span(colBound * inBlockSize.second, inY.n_cols - 1)
            );
            result(result.n_rows - 1, result.n_cols - 1) = arma::mean(arma::vectorise(block));
        }

        return result;
    }

    CubeFloat_t downscale(const CubeFloat_t & inY, const std::pair<size_t,size_t> inBlockSize)
    {
        // If block size is (1,1), no downsampling is needed
        if (inBlockSize.first == 1 && inBlockSize.second == 1)
        {
            return inY;
        }

        // Check if block size cleanly divides input
        std::pair<size_t, size_t> padding = {0, 0}; // bottom, right
        if (inY.n_rows % inBlockSize.first != 0)
        {
            padding.first = inBlockSize.first - (inY.n_rows % inBlockSize.first);
        }
        if (inY.n_cols % inBlockSize.second != 0)
        {
            padding.second = inBlockSize.second - (inY.n_cols % inBlockSize.second);
        }

        CubeFloat_t result(
            (inY.n_rows + padding.first) / inBlockSize.first, 
            (inY.n_cols + padding.second) / inBlockSize.second,
            inY.n_slices
        );

        for (size_t sliceIdx = 0; sliceIdx < inY.n_slices; ++sliceIdx)
        {
            result.slice(sliceIdx) = downscale(inY.slice(sliceIdx), inBlockSize);
        }
        return result;
    }


    void computeB(
        const MatrixFloat_t & b0, 
        const arma::SpMat<float> & W, 
        CubeFloat_t & B, 
        const size_t inSpatialDsFactor)
    {
        // Simplified case for no downsampling factor
        if (inSpatialDsFactor == 1)
        {
            CubeFloat_t delta = B.each_slice() - b0;
            CubeFloat_t dot = matrixToCubeByCol(W * cubeToMatrixBySlice(delta), B.n_rows, B.n_cols);
            B = -b0 - dot.each_slice();
            return;
        }

        CubeFloat_t delta = downscale(B, {inSpatialDsFactor, inSpatialDsFactor});
        MatrixFloat_t downscaledb0 = downscale(b0, {inSpatialDsFactor, inSpatialDsFactor});
        delta.each_slice() -= downscaledb0;

        MatrixFloat_t dot = W * cubeToMatrixBySlice(delta); // d/4 x T

        CubeFloat_t dotCube = matrixToCubeByCol(
            dot, (B.n_rows - 1) / inSpatialDsFactor + 1, (B.n_cols - 1) / inSpatialDsFactor + 1
        );

        // Upsample
        CubeFloat_t upsampled(dotCube.n_rows * inSpatialDsFactor, 
                               dotCube.n_cols * inSpatialDsFactor, dotCube.n_slices);
        for (size_t sliceIdx = 0; sliceIdx < upsampled.n_slices; ++sliceIdx)
        {
            for (size_t rowIdx = 0; rowIdx < dotCube.n_rows; ++rowIdx)
            {
                for (size_t colIdx = 0; colIdx < dotCube.n_cols; ++colIdx)
                {
                    upsampled(
                        arma::span(rowIdx * inSpatialDsFactor, (rowIdx + 1) * inSpatialDsFactor - 1),
                        arma::span(colIdx * inSpatialDsFactor, (colIdx + 1) * inSpatialDsFactor - 1),
                        arma::span(sliceIdx)
                    ) = arma::ones<MatrixFloat_t>(inSpatialDsFactor, inSpatialDsFactor) * dotCube(rowIdx, colIdx, sliceIdx);
                }
            }
        }
    
        B = -upsampled(arma::span(0, B.n_rows - 1), arma::span(0, B.n_cols - 1), arma::span::all);
        B = -b0 + B.each_slice();
    }

    void greedyCorr(
        const CubeFloat_t & inY,
        CubeFloat_t & outA,
        MatrixFloat_t & outC,
        MatrixFloat_t & outRawC,
        arma::SpMat<float> & outSpatialB,
        MatrixFloat_t & outTemporalB,
        MatrixFloat_t & inOutNoise,
        DeconvolutionParams inDeconvParams,
        InitializationParams inInitParams,
        SpatialParams inSpatialParams,
        const int32_t maxNumNeurons,
        const float ringSizeFactor,
        const float mergeThresh,
        const size_t numIterations,
        const size_t inNumThreads,
        const bool outputFinalTraces)
    {
        /* Greedy corr consists of 15 steps listed below:
             1.  Noise estimation
             2.  Initialization
             3.  Background estimation
             4.  Spatial updates
             5.  Temporal updates
             6.  Initialization (from the residuals)
             7.  Merging
             8.  Spatial updates
             9.  Temporal updates
             10. Background estimation
             11. Merging
             12. Spatial updates
             13. Raw traces
             14. Temporal updates
             15. Remove empty components
        */

        // matY points to the same memory as inY
        const MatrixFloat_t matY(
            const_cast<float*>(inY.memptr()),
            inY.n_rows * inY.n_cols,
            inY.n_slices,
            false,
            true);

        if (inOutNoise.empty())
        {
            isx::getNoiseFft(inY, inOutNoise, inDeconvParams.m_noiseRange, inDeconvParams.m_noiseMethod);
        }

        // estimate appropriate morphological filter sizes based on cell diameter
        if (inInitParams.m_gSig < 2)
        {
            inInitParams.m_gSig = static_cast<int32_t>((static_cast<float>(inInitParams.m_gSiz - 1)) / 4.0f);
        }
        if (inSpatialParams.m_closingKSize < 2)
        {
            inSpatialParams.m_closingKSize = static_cast<int32_t>((static_cast<float>(inInitParams.m_gSiz - 1)) / 4.0f);
        }

        // Only use second order AR for final update temporal components call
        const bool isFirstOrderAr = inDeconvParams.m_firstOrderAR;        
        inDeconvParams.m_firstOrderAR = true;

        {
            MatrixFloat_t outCRaw, tmpS;
            initNeuronsCorrPNR(inY, outA, outC, outCRaw, tmpS, inDeconvParams, inInitParams, maxNumNeurons);
        }

        MatrixFloat_t matB = matY - cubeToMatrixBySlice(outA) * outC;

        // cubeB points to the same memory as matB 
        CubeFloat_t cubeB(
            matB.memptr(),
            inY.n_rows,
            inY.n_cols,
            inY.n_slices,
            false,
            true);
        
        {
            arma::SpMat<float> W;
            ColumnFloat_t B0;
            std::pair<size_t, size_t> inDims(inY.n_rows, inY.n_cols);

            computeW(matY, cubeToMatrixBySlice(outA), outC, inDims, ringSizeFactor * inInitParams.m_gSiz, 
                     W, B0, inSpatialParams.m_bgSsub, inNumThreads);

            computeB(arma::reshape(B0, inY.n_rows, inY.n_cols), W, cubeB, inSpatialParams.m_bgSsub);
            cubeB += inY;
        }

        updateSpatialComponents(cubeB, outA, outC, inOutNoise, inSpatialParams.m_closingKSize, inSpatialParams.m_pixelsPerProc, inNumThreads);

        {
            // temporary variables not needed beyond this step
            ColumnFloat_t tmpBl, tmpC1, tmpSn;
            MatrixFloat_t tmpG, tmpYrA, tmpS;

            updateTemporalComponents(
                matB, cubeToMatrixBySlice(outA), outC, tmpBl, tmpC1, tmpG, tmpSn, tmpS, tmpYrA,
                inDeconvParams, 2, inNumThreads);
        }

        for (size_t iter = 0; iter < numIterations - 1; ++iter)
        {
            // maxNumNeurons is the global allowable number of neurons
            // Get number of additional neurons that can be found based on the current count
            int32_t maxNumNewNeurons = std::max(maxNumNeurons - static_cast<int32_t>(outA.n_slices), 0);
            if (maxNumNewNeurons > 0 || maxNumNeurons == 0)
            {
                CubeFloat_t input = matrixToCubeByCol(matB - cubeToMatrixBySlice(outA) * outC, inY.n_rows, inY.n_cols);
                CubeFloat_t outAR;
                MatrixFloat_t outCR, outCRRaw, tmpS;
                initNeuronsCorrPNR(input, outAR, outCR, outCRRaw, tmpS, inDeconvParams, inInitParams, maxNumNewNeurons);

                outA = arma::join_slices(outA, outAR);
                outC = arma::join_cols(outC, outCR);
            }
            else
            {
                break;
            }
        }

        {
            MatrixFloat_t tmpRawC;
            MatrixFloat_t matA = cubeToMatrixBySlice(outA);
            mergeComponents(matA, outC, tmpRawC, mergeThresh, inDeconvParams, inNumThreads);
            outA = matrixToCubeByCol(matA, inY.n_rows, inY.n_cols);
        }

        updateSpatialComponents(cubeB, outA, outC, inOutNoise, inSpatialParams.m_closingKSize, inSpatialParams.m_pixelsPerProc, inNumThreads);

        {
            // temporary variables not needed beyond this step
            ColumnFloat_t tmpBl, tmpC1, tmpSn;
            MatrixFloat_t tmpG, tmpYrA, tmpS;

            updateTemporalComponents(
                matB, cubeToMatrixBySlice(outA), outC, tmpBl, tmpC1, tmpG, tmpSn, tmpS, tmpYrA,
                inDeconvParams, 2, inNumThreads);
        }

        {
            arma::SpMat<float> W;
            ColumnFloat_t B0;
            std::pair<size_t,size_t> inDims(inY.n_rows, inY.n_cols);
            computeW(matY, cubeToMatrixBySlice(outA), outC, inDims, ringSizeFactor * inInitParams.m_gSiz,
                     W, B0, inSpatialParams.m_bgSsub, inNumThreads);

            matB = matY - cubeToMatrixBySlice(outA) * outC;
            computeB(arma::reshape(B0, inY.n_rows, inY.n_cols), W, cubeB, inSpatialParams.m_bgSsub);

            outTemporalB = -cubeToMatrixBySlice(cubeB);
            cubeB += inY;
        }

        {
            MatrixFloat_t tmpRawC;
            MatrixFloat_t matA = cubeToMatrixBySlice(outA);
            mergeComponents(matA, outC, tmpRawC, mergeThresh, inDeconvParams, inNumThreads);
            outA = matrixToCubeByCol(matA, inY.n_rows, inY.n_cols);
        }

        updateSpatialComponents(cubeB, outA, outC, inOutNoise, 1, inSpatialParams.m_pixelsPerProc, inNumThreads);

        {
            // Compute trace residual YrA
            MatrixFloat_t matA = cubeToMatrixBySlice(outA);
            ColumnFloat_t nA = arma::sum(arma::square(matA)).t() + std::numeric_limits<float>::epsilon();
            MatrixFloat_t YA = (matA.t() * matB).t() * arma::diagmat(1.0f / nA);
            MatrixFloat_t AA = (matA.t() * matA) * arma::diagmat(1.0f / nA);
            MatrixFloat_t YrA = YA.t() - (AA.t() * outC);

            // Raw C combines trace residual with current estimation of C
            outRawC = YrA + outC;
        }

        if (outputFinalTraces)
        {
            // Use input AR order settings for last temporal component update
            inDeconvParams.m_firstOrderAR = isFirstOrderAr;

            // temporary variables not needed beyond this step
            ColumnFloat_t tmpBl, tmpC1, tmpSn;
            MatrixFloat_t tmpG, tmpYrA, tmpS;
            
            updateTemporalComponents(
                matB, cubeToMatrixBySlice(outA), outC, tmpBl, tmpC1, tmpG, tmpSn, tmpS, tmpYrA,
                inDeconvParams, 2, inNumThreads);
        }

        // remove empty components
        removeEmptyComponents(outA, outC, outRawC);
    }

} // namespace isx
