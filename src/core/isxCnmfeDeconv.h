#ifndef ISX_CNMFE_DECONV_H
#define ISX_CNMFE_DECONV_H

#include "isxArmaUtils.h"
#include "isxCnmfeNoise.h"
#include "isxCnmfeParams.h"

namespace isx
{
    /// Infer the most likely discretized spike train underlying a fluorescence trace
    ///
    /// \param inTrace          Column vector containing fluorescent trace intensities
    /// \param inOutARParams    Parameters of AR process. Estimated if vector is empty
    /// \param inOutNoise       Standard deviation of noise distribution. Estimated if negative value given
    /// \param outCaTrace       The inferred denoised fluorescence signal
    /// \param outBaseline      Estimated fluorescence baseline value
    /// \param outInitCaVal     Estimated initial calcium value
    /// \param outSpikes        Discretized deconvolved neural activity (spikes)
    /// \param inDeconvParam    Parameters for estimating noise and AR params for deconvolution.
    void constrainedFoopsi(
        const ColumnFloat_t & inTrace,
        std::vector<float> & inOutARParams,
        float & inOutNoise,
        ColumnFloat_t & outCaTrace,
        float & outBaseline,
        float & outInitCaVal,
        ColumnFloat_t & outSpikes,
        DeconvolutionParams inDeconvParams = DeconvolutionParams());

    /// Estimates the time constants of the autoregressive model
    ///
    /// \param inTrace          Column vector containing fluorescent trace intensities
    /// \param inNoise          Standard deviation of noise distribution.
    /// \param inLags           Number of lags for estimating time constants
    /// \param inFudgeFactor    Fudge factor for reducing time constant bias
    /// \param inFirstOrderAR   If true, use AR(1) model. Otherwise, use AR(2)
    /// \param outParams        Estimated parameters of AR process.
    void estimateTimeConst(
        const ColumnFloat_t & inTrace,
        const float inNoise,
        const uint32_t inLags,
        const float inFudgeFactor,
        const bool inFirstOrderAR,
        std::vector<float> & outParams);

    /// Compute from the raw traces the corresponding deconvolved traces and inferred spikes
    ///
    /// \param inRawC           Column vector containing the raw fluorescence trace
    /// \param outC             Column vector containing the deconvolved fluorescence trace
    /// \param outS             Discretized deconvolved neural activity (spikes)
    /// \param outSn            Standard deviation of the noise distribution. Estimated if a negative value is provided
    /// \param inDeconvParams   Parameters for estimating noise and autoregressive model used for deconvolution
    /// \param inNumIterations  Number of deconvolution iteration to perform
    void deconvolveTraces(
        const MatrixFloat_t & inRawC,
        MatrixFloat_t & outC,
        MatrixFloat_t & outS,
        ColumnFloat_t & outSn,
        DeconvolutionParams inDeconvParams,
        const size_t inNumIterations = 1);

} // namespace isx

#endif //ISX_CNMFE_DECONV_H
