#include "isxCnmfePatch.h"
#include "isxCnmfeCore.h"
#include "isxCnmfeMerging.h"
#include "isxCnmfeNoise.h"
#include "isxMemoryMappedFileUtils.h"
#include "isxCnmfeUtils.h"
#include "isxCnmfeParams.h"
#include "isxUtilities.h"
#include "isxLog.h"

#include "ThreadPool.h"

namespace isx
{
    void removeDuplicates(
        Cnmfe & cnmfe,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & patchCoordinates,
        const std::vector<std::pair<float,float>> & patchCenters,
        const size_t patchId)
    {
        size_t numComponents = cnmfe.getNumNeurons();
        arma::uvec indicesToKeep(numComponents);
        size_t keepCount = 0;

        for (size_t i=0; i < numComponents; i++)
        {
            // neuron center
            std::pair<float,float> neuronCenter = computeCentroid(cnmfe.getSpatialComponents().slice(i));
            neuronCenter.first += std::get<0>(patchCoordinates[patchId]);
            neuronCenter.second += std::get<2>(patchCoordinates[patchId]);

            // distance between neuron center and patch centers
            size_t closestPatch = patchId;
            float shortestDistance = std::numeric_limits<float>::max();
            for (size_t k=0; k < patchCenters.size(); k++)
            {
                ColumnFloat_t diffVec({neuronCenter.first - patchCenters[k].first, neuronCenter.second - patchCenters[k].second});
                float distance = arma::norm(diffVec);

                if (distance < shortestDistance || (distance == shortestDistance && k == patchId))
                {
                    shortestDistance = distance;
                    closestPatch = k;
                }
            }

            // keep component only if shortest distance is to current patch
            if (closestPatch == patchId)
            {
                indicesToKeep(keepCount) = i;
                keepCount += 1;
            }
        }

        indicesToKeep = indicesToKeep.head(keepCount);
        cnmfe.setSpatialComponents(cnmfe.getSpatialComponents().slices(indicesToKeep));
        cnmfe.setTemporalComponents(cnmfe.getTemporalComponents().rows(indicesToKeep));
        cnmfe.setRawTemporalComponents(cnmfe.getRawTemporalComponents().rows(indicesToKeep));
    }

    void getPatchCoordinates(
        std::vector<std::tuple<size_t,size_t,size_t,size_t>> & patchCoordinates,
        std::vector<std::pair<float,float>> & patchCenters,
        const CnmfeMode_t & inMode,
        const size_t inNumRows,
        const size_t inNumCols,
        const size_t patchSize,
        const size_t overlap,
        const size_t border)
    {
        if (inMode == CnmfeMode_t::ALL_IN_MEMORY
            || (patchSize >= inNumRows && patchSize >= inNumCols))
        {
            patchCoordinates.emplace_back(std::make_tuple(0, inNumRows - 1, 0, inNumCols - 1));
            if (inMode != CnmfeMode_t::ALL_IN_MEMORY)
            {
                ISX_LOG_INFO("Patch size larger than field of view, using single patch");
            }
            return;
        }

        size_t rowIdx = border;
        size_t lastRowIdx = inNumRows-1-border;
        while(rowIdx < lastRowIdx)
        {
            size_t rowMin, rowMax;
            if (rowIdx == border)
            {
                // first row
                rowMin = border;
                rowMax = rowMin + patchSize - 1;
            }
            else if (rowIdx - overlap + patchSize >= lastRowIdx)
            {
                // last row
                rowMax = lastRowIdx;
                rowMin = rowMax - patchSize + 1;
            }
            else
            {
                rowMin = rowIdx - overlap;
                rowMax = rowMin + patchSize;
            }

            size_t colIdx = border;
            size_t lastColIdx = inNumCols-1-border;
            while (colIdx < lastColIdx)
            {
                size_t colMin, colMax;
                if (colIdx == border)
                {
                    // first column
                    colMin = border;
                    colMax = colMin + patchSize - 1;
                }
                else if (colIdx - overlap + patchSize >= lastColIdx)
                {
                    // last column
                    colMax = lastColIdx;
                    colMin = colMax - patchSize + 1;
                }
                else
                {
                    colMin = colIdx - overlap;
                    colMax = colMin + patchSize;
                }

                colIdx = colMax;
                patchCoordinates.emplace_back(rowMin, rowMax, colMin, colMax);
                patchCenters.emplace_back(static_cast<float>((rowMax - rowMin)/2.0 + rowMin), static_cast<float>((colMax - colMin)/2.0 + colMin));
            }
            rowIdx = rowMax;
        }
    }

    void patchCnmfeParallel(
        Cnmfe & cnmfe,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & patchCoordinates,
        const std::vector<std::pair<float,float>> & patchCenters,
        const size_t patchId,
        const std::string memoryMapFilename,
        const size_t numRows,
        const size_t numCols,
        const size_t numFrames,
        const DataType dataType)
    {
        CubeFloat_t fov;
        readMemoryMappedFileMovie(
            memoryMapFilename,
            numRows,
            numCols,
            numFrames,
            dataType,
            patchCoordinates[patchId],
            fov);
        
        cnmfe.fit(fov);

        if (patchCoordinates.size() > 1)
        {
            removeDuplicates(cnmfe, patchCoordinates, patchCenters, patchId);
        }
    }

    void mergePatchResults(
        CubeFloat_t & outA,
        MatrixFloat_t & outRawC,
        const size_t numRows,
        const size_t numCols,
        const size_t numFrames,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & rois,
        std::vector<Cnmfe> & cnmfes,
        const DeconvolutionParams inDeconvParams,
        const float mergeThresh,
        const size_t numThreadsOverride,
        size_t numComponents)
    {
        // merge results from all regions of interest
        outA = arma::zeros<CubeFloat_t>(numRows, numCols, numComponents);
        outRawC = arma::zeros<MatrixFloat_t>(numComponents, numFrames);
        MatrixFloat_t outC = arma::zeros<MatrixFloat_t>(numComponents, numFrames);
        size_t curCompIdx = 0;
        for (size_t i=0; i < rois.size(); i++)
        {
            std::tuple<size_t,size_t,size_t,size_t> roi = rois[i];
            size_t n = cnmfes[i].getNumNeurons();

            outA(arma::span(std::get<0>(roi), std::get<1>(roi)),
                 arma::span(std::get<2>(roi), std::get<3>(roi)),
                 arma::span(curCompIdx, curCompIdx + n - 1)) = cnmfes[i].getSpatialComponents();
            outC.rows(arma::span(curCompIdx, curCompIdx + n - 1)) = cnmfes[i].getTemporalComponents();
            outRawC.rows(arma::span(curCompIdx, curCompIdx + n - 1)) = cnmfes[i].getRawTemporalComponents();

            curCompIdx += n;
        }

        // when there are multiple patches, merge components to deal with overlaps
        // last round of deconvolution skipped after merging to preserve the raw traces
        if (rois.size() > 1)
        {
            MatrixFloat_t matA = cubeToMatrixBySlice(outA);
            int mergingOperations = 5; // empirically chosen to prevent infinite merging loop
            bool compsMerged = true;
            while (compsMerged && mergingOperations > 0){
                compsMerged = mergeComponents(matA, outC, outRawC, mergeThresh, inDeconvParams, numThreadsOverride);
                mergingOperations--;
            }
            outA = matrixToCubeByCol(matA, numRows, numCols);
        }
    }

    void patchCnmfe(
        const SpTiffMovie_t & inMovie,
        const std::string inMemoryMapPath,
        CubeFloat_t & outA,
        MatrixFloat_t & outRawC,
        const DeconvolutionParams inDeconvParams,
        InitializationParams inInitParams,
        const SpatialParams inSpatialParams,
        const PatchParams inPatchParams,
        const int32_t maxNumNeurons,
        const float ringSizeFactor,
        const float mergeThresh,
        const size_t numIterations,
        const size_t numThreads,
        const CnmfeOutputType_t outputType)
    {
        ISX_LOG_INFO("Using ", cnmfeModeNameMap.at(inPatchParams.m_mode), " processing mode");

        const size_t numRows = inMovie->getFrameHeight();
        const size_t numCols = inMovie->getFrameWidth();
        const size_t numFrames = inMovie->getNumFrames();
        const DataType dataType = inMovie->getDataType();

        std::vector<std::tuple<size_t, size_t, size_t, size_t>> patchCoordinates;
        std::vector<std::pair<float,float>> patchCenters;
        getPatchCoordinates(
            patchCoordinates,
            patchCenters,
            inPatchParams.m_mode,
            numRows,
            numCols,
            inPatchParams.m_patchSize,
            inPatchParams.m_overlap,
            inInitParams.m_boundaryDist);
        ISX_LOG_INFO("Field of view divided into ", patchCoordinates.size(), patchCoordinates.size() > 1 ? " patches" : " patch");

        ISX_LOG_INFO("Creating temporary binary file for memory mapping movie (file: ", inMemoryMapPath, ")");
        writeMemoryMappedFileMovie(
            inMovie,
            inMemoryMapPath);

        // border applied to whole FOV, therefore set to 0 for patches
        if (inPatchParams.m_mode == CnmfeMode_t::PATCH_PARALLEL || inPatchParams.m_mode == CnmfeMode_t::PATCH_SEQUENTIAL)
        {
            inInitParams.m_boundaryDist = 0;
        }

        // patches are processed in parallel by running cnmfe in single-thread mode (aka all-in-memory) on each
        const size_t numThreadsOverride = (inPatchParams.m_mode == CnmfeMode_t::PATCH_PARALLEL) ? 1 : numThreads;
        const bool outputFinalTraces = (inPatchParams.m_mode != CnmfeMode_t::ALL_IN_MEMORY);

        ISX_LOG_INFO("Launching CNMFe workers");
        size_t numComponents = 0;
        size_t numPatches = patchCoordinates.size();
        std::vector<Cnmfe> cnmfes(numPatches);
        for (size_t patchId=0; patchId < numPatches; patchId++)
        {
            cnmfes[patchId] = Cnmfe(inDeconvParams, inInitParams, inSpatialParams, maxNumNeurons, ringSizeFactor,
                                    mergeThresh, numIterations, numThreadsOverride, outputFinalTraces);
        }

        if (inPatchParams.m_mode == CnmfeMode_t::PATCH_PARALLEL)
        {
            // process regions of interest in parallel
            ThreadPool pool(numThreads);
            std::vector<std::future<void>> results(numPatches);
            for (size_t patchId = 0; patchId < numPatches; ++patchId)
            {
                results[patchId] = pool.enqueue(
                    patchCnmfeParallel,
                    std::ref(cnmfes[patchId]),
                    std::cref(patchCoordinates),
                    std::cref(patchCenters),
                    patchId,
                    inMemoryMapPath,
                    numRows,
                    numCols,
                    numFrames,
                    dataType);
            }

            for (size_t patchId = 0; patchId < numPatches; ++patchId)
            {
                results[patchId].get();
            }
        }
        else
        {
            // process regions of interest sequentially
            for (size_t patchId=0; patchId < numPatches; patchId++)
            {
                patchCnmfeParallel(
                    cnmfes[patchId],
                    patchCoordinates,
                    patchCenters,
                    patchId,
                    inMemoryMapPath,
                    numRows,
                    numCols,
                    numFrames,
                    dataType);
            }
        }

        for (size_t patchId = 0; patchId < numPatches; patchId++)
        {
            numComponents += cnmfes[patchId].getNumNeurons();
        }

        ISX_LOG_INFO("Merging patch results");
        mergePatchResults(
            outA, outRawC, numRows, numCols, numFrames, patchCoordinates, cnmfes,
            inDeconvParams, mergeThresh, numThreads, numComponents);

        MatrixFloat_t tmpC;
        removeEmptyComponents(outA, outRawC, tmpC);

        ISX_LOG_INFO(outRawC.n_rows, " components were extracted");

        ISX_LOG_INFO("Scaling spatiotemporal components");
        scaleSpatialTemporalComponents(outA, outRawC, outputType, inDeconvParams);

        ISX_LOG_INFO("Removing temporary binary file for memory mapping movie (file:", inMemoryMapPath, ")");
        std::remove(inMemoryMapPath.c_str());
    }
}
