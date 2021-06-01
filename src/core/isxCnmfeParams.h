#ifndef ISX_CNMFE_PARAMS_H
#define ISX_CNMFE_PARAMS_H

#include "isxCnmfeNoise.h"

namespace isx
{
    /// Mode for running Cnmfe
    enum class CnmfeMode_t
    {
        ALL_IN_MEMORY = 0,
        PATCH_SEQUENTIAL,
        PATCH_PARALLEL
    };

    const static std::map<CnmfeMode_t, std::string> cnmfeModeNameMap =
    {
        {CnmfeMode_t::ALL_IN_MEMORY, "all in memory"},
        {CnmfeMode_t::PATCH_SEQUENTIAL, "sequential patches"},
        {CnmfeMode_t::PATCH_PARALLEL, "parallel patches"}
    };

    /// Output type for spatial and temporal components
    enum CnmfeOutputType_t
    {
        DF = 0,        // Components are normalized and temporal traces are scaled by the average pixel intensity value of the nth percentile brightest pixels
        NOISE_SCALED,  // Components are normalized and temporal traces are scaled by noise estimate
        NORMALIZED,    // Spatial footprints normalized to unit vectors, temporal traces scaled by the magnitude of spatial footprints
        NON_NORMALIZED // No normalization, components are output as is from greedyCorr
    };

    /// Sparse nonnegative deconvolution problem solver for temporal components
    enum DeconvolutionMethod_t
    {
        OASIS = 0 // Online active set optimization problem solver
    };

    struct DeconvolutionParams
    {
        DeconvolutionParams()
        {
        }

        DeconvolutionParams(
            std::pair<float, float> noiseRange,
            AveragingMethod_t noiseMethod,
            bool firstOrderAR,
            uint32_t lags,
            float fudgeFactor,
            DeconvolutionMethod_t method)
            : m_noiseRange(noiseRange)
            , m_noiseMethod(noiseMethod)
            , m_firstOrderAR(firstOrderAR)
            , m_lags(lags)
            , m_fudgeFactor(fudgeFactor)
            , m_method(method)
        {
        }

        std::pair<float, float>   m_noiseRange = {0.25f, 0.5f};
        AveragingMethod_t         m_noiseMethod = AveragingMethod_t::MEAN;
        bool                      m_firstOrderAR = true;
        uint32_t                  m_lags = 5;
        float                     m_fudgeFactor = 0.96f;
        DeconvolutionMethod_t     m_method = DeconvolutionMethod_t::OASIS;
    };

    struct InitializationParams
    {
        InitializationParams()
        {
        }

        InitializationParams(
            const bool deconvolve,
            const int32_t averageCellDiameter,
            const int32_t gaussianKernelSize,
            const float minCorr,
            const float minPNR,
            const int32_t minNumPixels,
            const int32_t boundaryDist,
            const int32_t noiseThreshold)
            : m_deconvolve(deconvolve)
            , m_averageCellDiameter(averageCellDiameter)
            , m_gaussianKernelSize(gaussianKernelSize)
            , m_minCorr(minCorr)
            , m_minPNR(minPNR)
            , m_minNumPixels(minNumPixels)
            , m_boundaryDist(boundaryDist)
            , m_noiseThreshold(noiseThreshold)
        {
        }

        bool m_deconvolve = true;           ///< If true, deconvolve in InitCorrPNR. Otherwise, don't.
        int32_t m_averageCellDiameter = 7;  ///< Average diameter of neuron
        int32_t m_gaussianKernelSize = 0;   ///< Width of Gaussian kernel used for spatial filtering (< 2 will be auto estimated)
        float  m_minCorr = 0.8f;            ///< Minimum local correlation coefficient for seed pixel
        float  m_minPNR = 10.0f;            ///< Minimum peak-to-noise ratio for seed pixel
        int32_t m_minNumPixels = 3;         ///< Minimum number of non-zero pixels for each neuron
        int32_t m_boundaryDist = 0;         ///< Minimum distance from image boundary for neuron initialization
        int32_t m_noiseThreshold = 2;       ///< Nose threshold used when computing local correlation image
    };

    struct SpatialParams
    {
        SpatialParams()
        {
        }

        SpatialParams(
            const size_t bgSsub,
            const size_t pixelsPerProc,
            const int32_t closingKSize)
            : m_bgSsub(bgSsub)
            , m_pixelsPerProc(pixelsPerProc)
            , m_closingKSize(closingKSize)
        {
        }

        size_t  m_bgSsub = 2;           ///< Background spatial downsampling factor
        size_t  m_pixelsPerProc = 1000; ///< Number of pixels to process in parallel at once
        int32_t m_closingKSize = 0;     ///< Morphological closing kernel size (< 2 will be auto estimated)
    };

    struct PatchParams
    {
        PatchParams()
        {
        }

        PatchParams(
            const int patchSize,
            const int overlap,
            const CnmfeMode_t mode)
            : m_patchSize(patchSize)
            , m_overlap(overlap)
            , m_mode(mode)
        {
        }

        size_t m_patchSize = 80;                                 ///< Size of an individual patch
        size_t m_overlap = 20;                                   ///< Amount of overlap between patches in pixels
        CnmfeMode_t m_mode = CnmfeMode_t::PATCH_PARALLEL;        ///< Cnmfe processing mode
    };

} // namespace isx

#endif //ISX_CNMFE_PARAMS_H
