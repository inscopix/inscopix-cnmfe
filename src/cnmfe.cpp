#include "isx/cnmfe.h"
#include "isxArmaUtils.h"
#include "isxCnmfeParams.h"
#include "isxCnmfeIO.h"
#include "isxTiffMovie.h"
#include "isxCnmfePatch.h"
#include "json.hpp"

namespace isx
{
    std::string getMemoryMapDirPath(
        const std::string & inputMoviePath,
        const std::string & outputDir,
        const int processingMode,
        const int patchSize,
        const int patchOverlap)
    {
        nlohmann::json params;
        params["absoluteFilepath"] = inputMoviePath;
        if (static_cast<isx::CnmfeMode_t>(processingMode) != CnmfeMode_t::ALL_IN_MEMORY)
        {
            // patch size and patch overlap don't matter for all-in-memory mode
            params["patchSize"] = patchSize;
            params["patchOverlap"] = patchOverlap;
        }

        // construct unique hash based on input parameters to uniquely identify memory map files
        std::stringstream id;
        id << std::hex << std::hash<std::string>{}(params.dump());
        return outputDir + "/" + getBaseName(inputMoviePath) + "-" + id.str();
    }

    // Generates an output filepath to store both footprints and traces (.h5)
    std::string getH5OutputFilename(const std::string & inputMoviePath, const std::string & outputDir)
    {
        return outputDir + "/" + getBaseName(inputMoviePath) + "_output.h5";
    }

    // Generates an output filepath to store footprints (.tiff stack)
    std::string getFootprintsOutputFilename(const std::string & inputMoviePath, const std::string & outputDir)
    {
        return outputDir + "/" + getBaseName(inputMoviePath) + "_footprints.tiff";
    }

    // Generates an output filepath to store traces (.csv)
    std::string getTracesOutputFilename(const std::string & inputMoviePath, const std::string & outputDir)
    {
        return outputDir + "/" + getBaseName(inputMoviePath) + "_traces.csv";
    }

    void cnmfe(
        const std::string & inputMoviePath,
        const std::string & outputDirPath,
        const int outputFiletype,
        const int averageCellDiameter,
        const float minCorr,
        const float minPnr,
        const int gaussianKernelSize,
        const int closingKernelSize,
        const int backgroundDownsamplingFactor,
        const float ringSizeFactor,
        const float mergeThreshold,
        const int numThreads,
        const int processingMode,
        const int patchSize,
        const int patchOverlap,
        const int traceOutputUnits)
    {
        if (!pathExists(outputDirPath))
        {
            makeDirectory(outputDirPath);
        }

        const SpTiffMovie_t movie = std::shared_ptr<TiffMovie>(new TiffMovie(inputMoviePath));

        // dir where memory-mapped files will be stored
        const std::string memoryMapDir = getMemoryMapDirPath(inputMoviePath, outputDirPath, processingMode, patchSize, patchOverlap);

        // prepare cnmfe parameters
        InitializationParams initParams;
        initParams.m_gSiz = averageCellDiameter * 2;
        initParams.m_minCorr = minCorr;
        initParams.m_minPNR = minPnr;
        initParams.m_gSig = gaussianKernelSize;

        SpatialParams spatialParams;
        spatialParams.m_bgSsub = backgroundDownsamplingFactor;
        spatialParams.m_closingKSize = closingKernelSize;

        DeconvolutionParams deconvParams;

        PatchParams patchParams;
        patchParams.m_mode = static_cast<CnmfeMode_t>(processingMode);
        patchParams.m_patchSize = patchSize;
        patchParams.m_overlap = patchOverlap;

        const int maxNumNeurons = 0;     // 0 for auto estimate
        const size_t numIterations = 2;  // empirically chosen as optimal speed/performance tradeoff

        const CnmfeOutputType_t outputType = static_cast<CnmfeOutputType_t>(traceOutputUnits);

        // run cnmfe
        CubeFloat_t footprints;  // spatial footprints
        MatrixFloat_t traces;    // raw temporal traces
        patchCnmfe(movie, memoryMapDir, footprints, traces, deconvParams, initParams, spatialParams, patchParams,
           maxNumNeurons, ringSizeFactor, mergeThreshold, numIterations, numThreads, outputType);

        // save output to disk
        if (outputFiletype == 1)
        {
            std::string outputFilename = getH5OutputFilename(inputMoviePath, outputDirPath);
            saveOutputToH5File(footprints, traces, outputFilename);
        }
        else
        {
            // default to tiff/csv if invalid output filetype is provided
            std::string footprintsOutputFilename = getFootprintsOutputFilename(inputMoviePath, outputDirPath);
            saveFootprintsToTiffFile(footprints, footprintsOutputFilename);

            std::string tracesOutputFilename = getTracesOutputFilename(inputMoviePath, outputDirPath);
            saveTracesToCSVFile(traces, tracesOutputFilename);
        }
    }
}
