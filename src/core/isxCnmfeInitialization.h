#ifndef ISX_CNMFE_INITIALIZATION_H
#define ISX_CNMFE_INITIALIZATION_H

#include "isxArmaUtils.h"
#include "isxCnmfeParams.h"

namespace isx
{
    /// Extracts spatial footprint and temporal activity of a neuron
    ///
    /// \param inDataFiltered        Filtered movie
    /// \param inDataRaw             Raw movie
    /// \param centerIndex           Coordinates of neuron center
    /// \param outAi                 Spatial footprint of neuron
    /// \param outCi                 Temporal activity of neuron
    /// \param minCorrNeuron         Min correlation between center and neuron pixels
    /// \param maxCorrBg             Max correlation between center and background pixels
    void extractAC(
        const CubeFloat_t & inDataFiltered,
        const CubeFloat_t & inDataRaw,
        const std::pair<int32_t,int32_t> centerIndex,
        MatrixFloat_t & outAi,
        ColumnFloat_t & outCi,
        const float minCorrNeuron = 0.9f,
        const float maxCorrBg = 0.3f);

    /// Initializes neurons from pixels with high local correlation and high peak-to-noise ratio
    ///
    /// \param inData               Input movie
    /// \param outA                 Spatial footprints of neurons
    /// \param outC                 Deconvolved and denoised temporal activity of neurons
    /// \param outCRaw              Denoised temporal activity of neurons
    /// \param outS                 Inferred spiking activity of neurons
    /// \param deconvolve           Boolean flag for deconvolving temporal traces
    /// \param inDeconvParams       Deconvolution parameters
    /// \param inInitParams         Initialization parameters
    /// \param maxNumNeurons        Maximum number of neurons to detect (0 is used for 'auto' which stops when all pixels are below initialization thresholds)
    void initNeuronsCorrPNR(
        const CubeFloat_t & inData,
        CubeFloat_t & outA,
        MatrixFloat_t & outC,
        MatrixFloat_t & outCRaw,
        MatrixFloat_t & outS,
        DeconvolutionParams inDeconvParams,
        InitializationParams inInitParams,
        int32_t maxNumNeurons = 0);
} // namespace isx

#endif //ISX_CNMFE_INITIALIZATION_H
