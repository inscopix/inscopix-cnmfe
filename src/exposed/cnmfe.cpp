#include "isx/cnmfe.h"
#include "isxArmaUtils.h"
#include "isxCnmfeIO.h"
#include "isxTiffMovie.h"
#include "isxCnmfePatch.h"
#include "isxLog.h"
#include "json.hpp"

namespace isx
{
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
        using nlohmann::json;

        const std::string logFileName = outputDirPath + "/" + "Inscopix_CNMFe_Log.txt";
        const std::string appName = "Inscopix CNMFe";
        const std::string appVersion = "1.0.0";
        Logger::initialize(logFileName, appName, appVersion);

        nlohmann::json params;
        params["inputMoviePath"] = inputMoviePath;
        params["outputDirPath"] = outputDirPath;
        params["outputFiletype"] = outputFiletype;
        params["averageCellDiameter"] = averageCellDiameter;
        params["minCorr"] = minCorr;
        params["minPnr"] = minPnr;
        params["gaussianKernelSize"] = gaussianKernelSize;
        params["closingKernelSize"] = closingKernelSize;
        params["backgroundDownsamplingFactor"] = backgroundDownsamplingFactor;
        params["ringSizeFactor"] = ringSizeFactor;
        params["mergeThreshold"] = mergeThreshold;
        params["numThreads"] = numThreads;
        params["processingMode"] = processingMode;
        params["patchSize"] = patchSize;
        params["patchOverlap"] = patchOverlap;
        params["traceOutputUnits"] = traceOutputUnits;
        ISX_LOG_INFO("CNMFe parameters:\n" + params.dump(4));

        if (!pathExists(outputDirPath))
        {
            makeDirectory(outputDirPath);
        }

        const SpTiffMovie_t movie = std::shared_ptr<TiffMovie>(new TiffMovie(inputMoviePath));

        // path where tmp memory-mapped file will be stored
        const std::string memoryMapPath = getMemoryMapPath(inputMoviePath, outputDirPath);

        // prepare cnmfe parameters
        InitializationParams initParams;
        initParams.m_averageCellDiameter = averageCellDiameter * 2;
        initParams.m_minCorr = minCorr;
        initParams.m_minPNR = minPnr;
        initParams.m_gaussianKernelSize = gaussianKernelSize;

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
        patchCnmfe(movie, memoryMapPath, footprints, traces, deconvParams, initParams, spatialParams, patchParams,
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
