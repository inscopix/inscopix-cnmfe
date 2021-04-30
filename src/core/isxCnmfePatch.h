#ifndef ISX_CNMFE_PATCH_H
#define ISX_CNMFE_PATCH_H

#include "isxCnmfeParams.h"
#include "isxTiffMovie.h"

namespace isx
{
    /// Run Cnmfe in patches through spatial division of the field of view
    ///
    /// \param inMovie              Input movie (d1 x d2 x T)
    /// \param inMemoryMapDir       Path to the directory where memory map files will be stored
    /// \param outA                 Spatial footprints (d1 x d2 x K)
    /// \param outRawC              Raw temporal activity traces (K x T)
    /// \param inDeconvParams       Deconvolution parameters
    /// \param inInitParams         Initialization parameters
    /// \param inSpatialParams      Spatial parameters
    /// \param inPatchParams        Patch parameters
    /// \param maxNumNeurons        Maximum number of neurons to detect (0 used for 'auto' which stops when all pixels are below initialization thresholds)
    /// \param ringSizeFactor       Ratio of ring radius to neuron diameter
    /// \param mergeThresh          Threshold for merging components
    /// \param numIterations        Number of iterations for neuron initialization
    /// \param numThreads           Number of threads to use when parallelization is possible
    /// \param outputType           Output type for spatial and temporal components
    void patchCnmfe(
        const SpTiffMovie_t & inMovie,
        const std::string inMemoryMapDir,
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
        const CnmfeOutputType_t traceOutputType = CnmfeOutputType_t::NON_NORMALIZED);
}

#endif //ISX_CNMFE_PATCH_H
