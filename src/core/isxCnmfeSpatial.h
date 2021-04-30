#ifndef ISX_CNMFE_SPATIAL_H
#define ISX_CNMFE_SPATIAL_H

#include "isxArmaUtils.h"

namespace isx
{
    /// Applies circular constraints to input image
    ///
    /// \param img    Input image
    void circularConstraint(MatrixFloat_t & img);

    /// Removes small non-zero pixels and disconnected components
    ///
    /// \param img           Input image
    /// \param inThresh      Threshold for pixel intensities relative to max pixel value
    /// \param inFilterSize  Filter size for morphological opening
    void connectivityConstraint(
        MatrixFloat_t & img,
        const float inThresh = 0.01,
        const int32_t inFilterSize = 5);

    /// Post-processing of a spatial component which includes the following steps
    ///
    /// (i)   Median filtering
    /// (ii)  Thresholding
    /// (iii) Morphological closing of spatial support
    /// (iv)  Extraction of largest connected component (to remove small unconnected pixels)
    ///
    /// \param inOutA           Matrix representing footprint of one component (d1 x d2)
    /// \param inCloseKSize     Filter size for morphological opening
    void thresholdComponentsParallel(
        MatrixFloat_t & inOutA,
        int32_t inCloseKSize = 3);

    /// Post-processing of spatial components. Calls thresholdComponentsParallel on each component.
    ///
    /// \param inOutA           Cube containing all spatial components (d1 x d2 x K)
    /// \param inCloseKSize     Filter size for morphological opening
    void thresholdComponents(
        CubeFloat_t & inOutA,
        int32_t inCloseKSize = 3);

    /// Dilates each spatial component and returns boolean array showing where components should be searched
    ///
    /// \param inA          Input cube containing all spatial components (d1 x d2 x K)
    /// \param outDistInd   Output cube of binary values showing where components should be searched
    void determineSearchLocation(const CubeFloat_t & inA, arma::ucube & outDistInd);

    /// Get indices of components that should be searched at each pixel
    ///
    /// \param inA          Input cube containing all spatial components (d1 x d2 x K)
    /// \param outDistInd   Output vector of vectors for each pixel indicating component indices to be searched
    void computeIndicator(const CubeFloat_t & inA, std::vector<arma::uvec>& outInd);

    /// Updates spatial footprints using Basis Pursuit Denoising (designed for parallel processing)
    ///
    /// \param inY              Input movie (w x h x t)
    /// \param inC              Temporal activity of neurons (n x t)
    /// \param inNoise          Matrix containing noise at each pixel
    /// \param inIndC           Vector of vectors for each pixel indicating component indices to be searched
    /// \param inPixelRange     Range of pixels to process
    /// \param inCct            Cross-correlation of temporal components
    /// \param outA             Output matrix containing spatial footprints for the input pixel group
    void regressionParallel(
        const CubeFloat_t & inY,
        const MatrixFloat_t & inC,
        const MatrixFloat_t & inNoise,
        const std::vector<arma::uvec> & inIndC,
        const std::pair<size_t, size_t> inPixelRange,
        const ColumnFloat_t & inCct,
        MatrixFloat_t & outA);

    /// Updates spatial footprints using Basis Pursuit Denoising
    ///
    /// \param inY                  Input movie (d1 x d2 x T)
    /// \param inOutA               Spatial components (d1 x d2 x K)
    /// \param inOutC               Temporal components (K x T)
    /// \param inNoise              Matrix containing noise at each pixel
    /// \param inPixelsPerProcess   Number of pixels to process per thread (if using multithreading)
    /// \param inNumThreads         Number of worker threads to run regression with
    void updateSpatialComponents(
        const CubeFloat_t & inY,
        CubeFloat_t & inOutA,
        MatrixFloat_t & inOutC,
        const MatrixFloat_t & inNoise,
        const int32_t inCloseKSize = 3,
        const size_t inPixelsPerProcess = 128,
        const size_t inNumThreads = 1);
} // namespace isx

#endif //ISX_CNMFE_SPATIAL_H
