#include "isxCnmfeCore.h"
#include "isxCnmfeGreedy.h"
#include "isxLog.h"

#include <thread>

namespace isx
{
    Cnmfe::Cnmfe()
    {
    }

    Cnmfe::Cnmfe(
        const DeconvolutionParams inDeconvParams,
        const InitializationParams inInitParams,
        const SpatialParams inSpatialParams,
        const int32_t maxNumNeurons,
        const float ringSizeFactor,
        const float mergeThresh,
        const size_t numIterations,
        const size_t numThreads,
        const bool outputFinalTraces)
        : m_deconvParams(inDeconvParams)
        , m_initParams(inInitParams)
        , m_spatialParams(inSpatialParams)
        , m_maxNumNeurons(maxNumNeurons)
        , m_ringSizeFactor(ringSizeFactor)
        , m_mergeThresh(mergeThresh)
        , m_numIterations(numIterations)
        , m_numThreads(numThreads)
        , m_outputFinalTraces(outputFinalTraces)
    {
    }

    void Cnmfe::fit(const CubeFloat_t & inY)
    {
        ISX_LOG_INFO(m_numThreads, m_numThreads > 1 ? " threads" : " thread", " assigned for fitting CNMF-E model");

        // estimate noise in each pixel
        ISX_LOG_INFO("Estimating individual pixel noise");
        isx::getNoiseFft(inY, m_noise, m_deconvParams.m_noiseRange, m_deconvParams.m_noiseMethod);

        // estimate spatiotemporal components using the greedy correlation approach
        greedyCorr(
            inY,
            m_A,
            m_C,
            m_rawC,
            m_spatialB,
            m_temporalB,
            m_noise,
            m_deconvParams,
            m_initParams,
            m_spatialParams,
            m_maxNumNeurons,
            m_ringSizeFactor,
            m_mergeThresh,
            m_numIterations,
            m_numThreads,
            m_outputFinalTraces
        );
    }

    const CubeFloat_t & Cnmfe::getSpatialComponents() const
    {
        return m_A;
    }

    const MatrixFloat_t & Cnmfe::getTemporalComponents() const
    {
        return m_C;
    }

    const MatrixFloat_t & Cnmfe::getRawTemporalComponents()
    {
        return m_rawC;
    }

    const arma::SpMat<float> & Cnmfe::getSpatialBackground() const
    {
        return m_spatialB;
    }

    const MatrixFloat_t & Cnmfe::getTemporalBackground() const
    {
        return m_temporalB;
    }

    void Cnmfe::setSpatialComponents(const CubeFloat_t & spatialComponents)
    {
        m_A = spatialComponents;
    }

    void Cnmfe::setTemporalComponents(const MatrixFloat_t & temporalComponents)
    {
        m_C = temporalComponents;
    }

    void Cnmfe::setSpatialBackground(const arma::SpMat<float> & spatialBackground)
    {
        m_spatialB = spatialBackground;
    }

    void Cnmfe::setTemporalBackground(const MatrixFloat_t & temporalBackground)
    {
        m_temporalB = temporalBackground;
    }

    void Cnmfe::setRawTemporalComponents(const MatrixFloat_t & rawTemporalComponents)
    {
        m_rawC = rawTemporalComponents;
    }

    size_t Cnmfe::getNumNeurons()
    {
        return m_A.n_slices;
    }

    size_t Cnmfe::getNumFrames()
    {
        return m_C.n_cols;
    }

    std::pair<size_t, size_t> Cnmfe::getNumPixels()
    {
        return std::make_pair(m_A.n_rows, m_A.n_cols);
    }

    const DeconvolutionParams & Cnmfe::getDeconvParams()
    {
        return m_deconvParams;
    }

    const InitializationParams & Cnmfe::getInitParams()
    {
        return m_initParams;
    }

    const SpatialParams & Cnmfe::getSpatialParams()
    {
        return m_spatialParams;
    }

    int32_t Cnmfe::getMaxNumNeurons()
    {
        return m_maxNumNeurons;
    }

    float Cnmfe::getRingSizeFactor()
    {
        return m_ringSizeFactor;
    }

    float Cnmfe::getMergeThreshold()
    {
        return m_ringSizeFactor;
    }

    size_t Cnmfe::getNumIterations()
    {
        return m_numIterations;
    }

    size_t Cnmfe::getNumThreads()
    {
        return m_numThreads;
    }
}
