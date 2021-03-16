#include "isxCnmfeTemporal.h"
#include "isxCnmfeUtils.h"
#include "ThreadPool.h"

namespace isx
{
    void constrainedFoopsiParallel(
        const ColumnFloat_t & inTrace,
        std::vector<float> & outArParams,
        float & outNoise,
        ColumnFloat_t & outC,
        float & outBaseline,
        float & outInitCaVal,
        ColumnFloat_t & outSpikes,
        DeconvolutionParams inDeconvParams)
    {
        ColumnFloat_t lCaTrace, lSpikes;
        constrainedFoopsi(inTrace, outArParams, outNoise, lCaTrace, outBaseline, outInitCaVal, lSpikes, inDeconvParams);

        ColumnFloat_t polynomial = arma::join_cols(ColumnFloat_t({1.0f}), -arma::conv_to<ColumnFloat_t>::from(outArParams));
        float gd = arma::max(arma::real(arma::roots(polynomial)));
        ColumnFloat_t gdVec(inTrace.n_elem);
        for (size_t idx = 0; idx < gdVec.n_elem; ++idx)
        {
            gdVec(idx) = std::pow(gd, static_cast<float>(idx));
        }

        outC = lCaTrace + outBaseline + (outInitCaVal * gdVec);
        outSpikes = lSpikes;
    }

    void updateIteration(
        MatrixFloat_t & YrA,       // (T x K)
        const MatrixFloat_t & AA,  // (K x K)
        MatrixFloat_t & inOutC,    // (K x T)
        MatrixFloat_t & outS,      // (K x T)
        ColumnFloat_t & outBl,
        ColumnFloat_t & outC1,
        ColumnFloat_t & outSn,
        MatrixFloat_t & outG,
        DeconvolutionParams inDeconvParams,
        const size_t inIterations,
        const size_t inNumThreads)
    {
        const size_t K = AA.n_rows;
        const size_t p = inDeconvParams.m_firstOrderAR ? 1 : 2;

        // Initialize outputs
        outS = arma::zeros<MatrixFloat_t>(arma::size(inOutC));
        outBl = ColumnFloat_t(K);
        outC1 = ColumnFloat_t(K);
        outSn = ColumnFloat_t(K);
        outG = MatrixFloat_t(p, K);

        MatrixFloat_t Ccopy = inOutC;

        // Group components into sets of non-overlapping components
        std::vector<std::vector<size_t>> components = updateOrderGreedy(AA);

        // Multiprocessing pool used for multithreading
        ThreadPool pool(inNumThreads);

        for (size_t iteration = 0; iteration < inIterations; ++iteration)
        {
            size_t cumComponentsUpdated = 0;
            for (const auto & comp : components)
            {
                const size_t compSize = comp.size();
                std::vector<ColumnFloat_t> lC(compSize);
                std::vector<ColumnFloat_t> lSp(compSize);

                std::vector<float> lBl(compSize);
                std::vector<float> lC1(compSize);
                std::vector<float> lNoise(compSize, -1.0f);
                std::vector<std::vector<float>> lArParams(compSize);

                if (inNumThreads < 2)
                {
                    // Naive implementation: Update each component one by one.
                    // Can be optimized by processing non-overlapping components in parallel
                    for (unsigned int compIdx = 0; compIdx < compSize; ++compIdx)
                    {
                        ColumnFloat_t Y = YrA.col(comp[compIdx]) + Ccopy.row(comp[compIdx]).t();
                        constrainedFoopsiParallel(
                            Y, lArParams[compIdx], lNoise[compIdx], lC[compIdx],
                            lBl[compIdx], lC1[compIdx], lSp[compIdx], inDeconvParams);
                    }
                }
                else
                {
                    // Process all components in this set in parallel using a thread pool
                    std::vector<ColumnFloat_t> Y(compSize);
                    std::vector<std::future<void>> results(compSize);
                    for (unsigned int compIdx = 0; compIdx < compSize; ++compIdx)
                    {
                        Y[compIdx] = YrA.col(comp[compIdx]) + Ccopy.row(comp[compIdx]).t();

                        results[compIdx] = pool.enqueue(
                            constrainedFoopsiParallel,
                            std::cref(Y[compIdx]),
                            std::ref(lArParams[compIdx]),
                            std::ref(lNoise[compIdx]),
                            std::ref(lC[compIdx]),
                            std::ref(lBl[compIdx]),
                            std::ref(lC1[compIdx]),
                            std::ref(lSp[compIdx]),
                            inDeconvParams);
                    }

                    for (unsigned int compIdx = 0; compIdx < compSize; ++compIdx)
                    {
                        results[compIdx].get();
                    }
                }

                // Update outputs with deconvolution outputs
                for (unsigned int compIdx = 0; compIdx < compSize; ++compIdx)
                {
                    YrA -= ((AA.row(comp[compIdx]).t() * (lC[compIdx].t() - inOutC.row(comp[compIdx])))).t();
                    inOutC(comp[compIdx], arma::span::all) = lC[compIdx].t();
                    outS(comp[compIdx], arma::span::all) = lSp[compIdx].t();
                    outBl(comp[compIdx]) = lBl[compIdx];
                    outC1(comp[compIdx]) = lC1[compIdx];
                    outSn(comp[compIdx]) = lNoise[compIdx];
                    outG(arma::span::all, comp[compIdx]) = arma::conv_to<ColumnFloat_t>::from(lArParams[compIdx]);
                }

                cumComponentsUpdated += compSize;
                // ISX_LOG_INFO("CNMFe: ", cumComponentsUpdated, " out of total ", K, " temporal components updated");
            }

            if (arma::norm(Ccopy - inOutC, "fro") <= 1e-3f * arma::norm(inOutC, "fro"))
            {
                // ISX_LOG_INFO("CNMFe: Temporal: Stopping - Overall temporal component not changing significantly");
                break;
            }
            else
            {
                Ccopy = inOutC;
            }
        }

    }

    void updateTemporalComponents(
        const MatrixFloat_t & inY,  // (d x T)
        const MatrixFloat_t & inA,  // (d x K)
        MatrixFloat_t & inOutC,     // (K x T)
        ColumnFloat_t & outBl,
        ColumnFloat_t & outC1,
        MatrixFloat_t & outG,
        ColumnFloat_t & outSn,
        MatrixFloat_t & outS,
        MatrixFloat_t & outYrA,
        DeconvolutionParams inDeconvParams,
        const size_t inIterations,
        const size_t inNumThreads)
    {
        ColumnFloat_t nA = arma::sum(arma::square(inA)).t() + std::numeric_limits<float>::epsilon();

        MatrixFloat_t YA = (inA.t() * inY).t() * arma::diagmat(1.0f / nA);
        MatrixFloat_t AA = (inA.t() * inA) * arma::diagmat(1.0f / nA);
        outYrA = YA - (AA.t() * inOutC).t();

        updateIteration(
            outYrA, AA, inOutC, outS, outBl, outC1, outSn, outG,
            inDeconvParams, inIterations, inNumThreads);

        // Remove empty temporal components
        // not currently implemented since this consists of removing traces that only contain zeros,
        // which never occurs for floating-point numbers after going through deconvolution

        outYrA = outYrA.t();
    }

} // namespace isx
