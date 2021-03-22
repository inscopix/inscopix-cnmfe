#ifndef ISX_CNMFE_GREEDY_H
#define ISX_CNMFE_GREEDY_H

#include "isxArmaUtils.h"
#include "isxCnmfeParams.h"
#include "isxCnmfeDeconv.h"
#include "isxCnmfeInitialization.h"

namespace isx
{
    /// Estimates fluctuating and constant background components using a ring model
    ///
    /// \param inY                  Input movie (d x T)
    /// \param inA                  Spatial footprints of neurons (d x K)
    /// \param inC                  Temporal activity of neurons (K x T)
    /// \param inDims               Dimensions of every frame (w x h)
    /// \param radius               Radius of the ring
    /// \param outW                 Estimate of weight matrix for fluctuating background (sparse d x d)
    /// \param outB0                Estimate of constant background baselines (d)
    /// \param spatialSub           Spatial subsampling factor
    /// \param inNumThreads         Threads to use when parallelization is possible
    void computeW(
        const MatrixFloat_t & inY,
        const MatrixFloat_t & inA,
        const MatrixFloat_t & inC,
        const std::pair<size_t,size_t> inDims,
        const float radius,
        arma::SpMat<float> & outW,
        ColumnFloat_t & outB0,
        const size_t spatialSub = 2,
        const size_t inNumThreads = 1);

    /// Average pooling, computing average for each block across the matrix
    ///
    /// \param inY          Matrix to be pooled
    /// \param inBlockSize  Dimensions of each block
    /// \return             Downscaled matrix
    MatrixFloat_t downscale(const MatrixFloat_t & inY, const std::pair<size_t,size_t> inBlockSize);

    /// Updates background matrix from weight matrix and constant background baselines
    ///
    /// \param b0                   Estimate of constant background baselines (d1 x d2)
    /// \param W                    Estimate of weight matrix for fluctuating background (d x d)
    /// \param B                    Estimate of background (d1 x d2 x T)
    /// \param inSpatialDsFactor    Spatial downsampling factor
    void computeB(
        const MatrixFloat_t & b0, 
        const arma::SpMat<float> & W, 
        CubeFloat_t & B, 
        const size_t inSpatialDsFactor);

    /// Initializes spatial footprints, temporal components, and background using a greedy correlation-based approach
    ///
    /// \param inY                  Input movie (d1 x d2 x T)
    /// \param outA                 Spatial footprints of neurons (d1 x d2 x K)
    /// \param outC                 Temporal activity trace of neurons (K x T)
    /// \param outRawC              Raw temporal activity of neurons (K x T) 
    /// \param outSpatialB          Background spatial components (d x d)
    /// \param outTemporalB         Background temporal components (d x T)
    /// \param inOutNoise           Noise estimation per pixel (d1 x d2)
    /// \param inDeconvParams       Deconvolution parameters
    /// \param inInitParams         Initialization parameters
    /// \param inSpatialParams      Spatial parameters
    /// \param maxNumNeurons        Maximum number of neurons to detect (0 is used for 'auto' which stops when all pixels are below initialization thresholds)
    /// \param ringSizeFactor       Ratio of ring radius to neuron diameter
    /// \param mergeThresh          Threshold for merging components
    /// \param numIterations        Number of iterations for initialization
    /// \param inNumThreads         Threads to use when parallelization is possible
    /// \param outputFinalTraces    Indicates whether to output final deconvolved traces (used in patch mode for merging components)
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
        const int32_t maxNumNeurons = 0,
        const float ringSizeFactor = 1.4f,
        const float mergeThresh = 0.85f,
        const size_t numIterations = 2,
        const size_t inNumThreads = 1,
        const bool outputFinalTraces = false);
} // namespace isx

#endif //ISX_CNMFE_GREEDY_H
