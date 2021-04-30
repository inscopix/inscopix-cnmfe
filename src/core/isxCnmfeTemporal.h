#ifndef ISX_CNMFE_TEMPORAL_H
#define ISX_CNMFE_TEMPORAL_H

#include "isxArmaUtils.h"
#include "isxCnmfeNoise.h"
#include "isxCnmfeDeconv.h"

namespace isx
{
    /// Helper function for calling constrainedFoopsi from temporal module
    void constrainedFoopsiParallel(
        const ColumnFloat_t & inTrace,
        std::vector<float> & outArParams,
        float & outNoise,
        ColumnFloat_t & outC,
        float & outBaseline,
        float & outInitCaVal,
        ColumnFloat_t & outSpikes,
        DeconvolutionParams inDeconvParams = DeconvolutionParams()
    );

    /// Helper function for updating temporal components using a block coordinate descent approach.
    void updateIteration(
        MatrixFloat_t & YrA,
        const MatrixFloat_t & AA,
        MatrixFloat_t & inOutC,
        MatrixFloat_t & outS,
        ColumnFloat_t & outBl,
        ColumnFloat_t & outC1,
        ColumnFloat_t & outSn,
        MatrixFloat_t & outG,
        DeconvolutionParams inDeconvParams = DeconvolutionParams(),
        const size_t inIterations = 2,
        const size_t inNumThreads = 1
    );

    /// Update temporal components given spatial components using a block coordinate descent approach.
    ///
    /// \param inY                  Input movie data (d x T)
    /// \param inA                  Matrix of spatial components (d x K)
    /// \param inOutC               Matrix of temporal components (K x T)
    /// \param outBl                Baseline for fluorescence trace of each component (K)
    /// \param outC1                Initial calcium concentration of each component (K)
    /// \param outG                 AR model parameters for each component (p, K)
    /// \param outSn                Noise level for each component (K)
    /// \param outS                 Spikes in deconvolved trace for each component (K x T)
    /// \param outYrA               Residual trace for each component (K x T)
    /// \param inDeconvParams       Deconvolution parameters for estimating noise and AR params
    /// \param inIterations         Maximum number of block coordinate descent loops
    /// \param inNumThreads         Number of worker threads to run deconvolution with
    void updateTemporalComponents(
        const MatrixFloat_t & inY,
        const MatrixFloat_t & inA,
        MatrixFloat_t & inOutC,
        ColumnFloat_t & outBl,
        ColumnFloat_t & outC1,
        MatrixFloat_t & outG,
        ColumnFloat_t & outSn,
        MatrixFloat_t & outS,
        MatrixFloat_t & outYrA, 
        DeconvolutionParams inDeconvParams = DeconvolutionParams(),
        const size_t inIterations = 2,
        const size_t inNumThreads = 1
    );

    /// Determines the update order of the temporal components using a greedy approach
    /// to find non overlapping spatial components.
    ///
    /// \param A    Input matrix representing spatial components. (K x K)
    inline std::vector<std::vector<size_t>> updateOrderGreedy(const MatrixFloat_t & A)
    {
        const size_t K = A.n_cols;
        std::vector<std::vector<size_t>> components;

        for (size_t i = 0; i < K; ++i)
        {
            bool newList = true;
            for (auto & list : components)
            {
                arma::uvec indices = arma::conv_to<arma::uvec>::from(list);
                // If no non-zero elements
                if (!arma::any(arma::vectorise(A.submat(arma::uvec({i}), indices))))
                {
                    list.push_back(i);
                    newList = false;
                    break;
                }
            }

            if (newList)
            {
                components.push_back({i});
            }
        }
        return components;
    }
} // namespace isx

#endif //ISX_CNMFE_TEMPORAL_H
