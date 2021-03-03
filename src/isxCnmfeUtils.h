#ifndef ISX_CNMFE_UTILS_H
#define ISX_CNMFE_UTILS_H

#include "isxArmaUtils.h"

namespace isx
{
    /// Computes the smallest power of 2 greater than or equal to n
    ///
    /// \param n    The input number
    /// \return     The next power of 2 greater than or equal to n
    uint32_t nextPowerOf2(const uint32_t n);

    /// Computes the autocovariance of inData at lag = -inMaxlag:0:inMaxlag
    ///
    /// \param inData    Column vector of data values
    /// \param outXcov   Column vector of autocovariances computed from -inMaxlag:0:inMaxlag
    /// \param inMaxlag  Number of lags to use in autocovariance calculation
    void autoCovariance(const ColumnFloat_t & inData, ColumnFloat_t & outXcov, const uint32_t inMaxlag = 5);

    /// Generates a decimation matrix
    ///
    /// \param inDims               Input dimensions
    /// \param subsamplingFactor    Subsampling factor
    arma::SpMat<float> generateDecimationMatrix(const std::pair<size_t,size_t> inDims, const size_t subsamplingFactor);

    /// Compute the centroid of a matrix
    ///
    /// \param inMatrix    Input matrix
    std::pair<float,float> computeCentroid(const MatrixFloat_t inMatrix);

    /// Computes the correlation image (8 neighbors for each pixeL) for inData using an optimized FFT-based method
    ///
    /// \param inData               Cube of movie data (h x w x t)
    /// \param outCorrMatrix        Matrix of cross-correlation with adjacent pixels
    void computeLocalCorr(const CubeFloat_t & inData, MatrixFloat_t & outCorrMatrix);
}

#endif //ISX_CNMFE_UTILS_H
