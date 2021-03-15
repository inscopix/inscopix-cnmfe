#ifndef ISX_CNMFE_MERGING_H
#define ISX_CNMFE_MERGING_H

#include "isxArmaUtils.h"
#include "isxCnmfeDeconv.h"

namespace isx
{
    /// Returns the Pearson correlation coefficient between two vectors
    ///
    /// \param X    Vector of x values
    /// \param Y    Vector of y values
    /// \return     The Pearson correlation coefficient
    static inline float pearsonr(const ColumnFloat_t & X, const ColumnFloat_t & Y)
    {
        float xmean = arma::mean(X);
        float ymean = arma::mean(Y);

        float normxm = arma::norm(X - xmean, 2);
        float normym = arma::norm(Y - ymean, 2);

        return std::max(std::min(arma::dot((X - xmean) / normxm, (Y - ymean) / normym ), 1.0f), -1.0f);
    }

    /// Depth-First-Search traversal to find connected components in undirected graph
    ///
    /// \param graph                    Adjacency matrix (input)
    /// \param numComponents            Number of connected components (output)
    /// \param connectedComponents      Vector containing the identity of each component (output)
    void connectedComponents(
        const arma::umat & graph,
        uint32_t & numComponents,
        arma::uvec & connectedComponents);

    /// Merges spatially overlapping components that have highly correlated temporal activity
    /// Returns true if some components were merged, false otherwise
    ///
    /// \param inOutA           Matrix of spatial components (d x K)
    /// \param inOutC           Matrix of temporal components (K x T)
    /// \param inOutRawC        Matrix of raw temporal components (K x T)
    /// \param inCorrThresh     Correlation threshold for merging
    /// \param inDeconvParams   Parameters for constrained foopsi parameter estimation
    /// \param inNumThreads     Number of worker threads to run merging with
    bool mergeComponents(
        MatrixFloat_t & inOutA,
        MatrixFloat_t & inOutC,
        MatrixFloat_t & inOutRawC,
        const float inCorrThresh = 0.85f,
        DeconvolutionParams inDeconvParams = DeconvolutionParams(),
        const size_t inNumThreads = 1
    );
} // namespace isx

#endif //ISX_CNMFE_MERGING_H
