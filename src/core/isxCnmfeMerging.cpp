#include "isxCnmfeMerging.h"
#include "isxCnmfeDeconv.h"
//#include "isxLog.h"
#include "ThreadPool.h"
#include <stack>


namespace isx
{
    void connectedComponents(
        const arma::umat & graph,
        uint32_t & numComponents,
        arma::uvec & connectedComponents)
    {
        int voidVal = -1;
        int endVal = -2;

        arma::SpMat<arma::uword> X(graph.t());
        arma::SpMat<arma::uword> XT(graph);
        X.sync();
        XT.sync();

        numComponents = 0;
        arma::ivec labels = arma::ivec(X.n_rows, arma::fill::ones) * voidVal;

        arma::ivec stack(labels);
        size_t stackHead;

        for(size_t vertex = 0; vertex < X.n_rows; ++vertex)
        {
            if (labels(vertex) == voidVal)
            {
                stackHead = vertex;
                stack[vertex] = endVal;

                while (static_cast<int>(stackHead) != endVal)
                {
                    size_t vertexIdx = stackHead;
                    stackHead = stack[vertexIdx];

                    labels[vertexIdx] = numComponents;

                    for (size_t j = X.col_ptrs[vertexIdx]; j < X.col_ptrs[vertexIdx + 1]; j++)
                    {
                        size_t tmpIdx = X.row_indices[j];
                        if (stack[tmpIdx] == voidVal)
                        {
                            stack[tmpIdx] = stackHead;
                            stackHead = tmpIdx;
                        }
                    }

                    for (size_t j = XT.col_ptrs[vertexIdx]; j < XT.col_ptrs[vertexIdx + 1]; j++)
                    {
                        size_t tmpIdx = XT.row_indices[j];
                        if (stack[tmpIdx] == voidVal)
                        {
                            stack[tmpIdx] = stackHead;
                            stackHead = tmpIdx;
                        }
                    }
                }

                numComponents++;
            }
        }
        connectedComponents = arma::conv_to<arma::uvec>::from(labels);
    }

    // Helper function to merge set of correlated components
    static void mergeIteration(
        const MatrixFloat_t & inA,
        const MatrixFloat_t & inC,
        const arma::uvec & inCompToMerge,
        std::vector<float> & outArParams,
        float & outNoise,
        ColumnFloat_t & outCaTrace,
        float & outBaseline,
        float & outInitCaVal,
        ColumnFloat_t & outSpikes,
        ColumnFloat_t & outA,
        ColumnFloat_t & outYrA,
        DeconvolutionParams inDeconvParams)
    {
        MatrixFloat_t A = inA.cols(inCompToMerge);
        MatrixFloat_t Ctmp = inC.rows(inCompToMerge);
        ColumnFloat_t Cnorm = arma::sqrt(arma::vectorise(arma::sum(arma::pow(A, 2))) % arma::sum(arma::pow(Ctmp, 2), 1));

        ColumnFloat_t computedA = A * Cnorm;
        ColumnFloat_t computedC;

        // epsilon is added in a few places below to prevent divisions by zero
        for (size_t idx = 0; idx < 10; ++idx)
        {
            computedC = (Ctmp.t() * (A.t() * computedA)) / (arma::dot(computedA, computedA) + std::numeric_limits<float>::epsilon());
            computedC.elem(arma::find(computedC < 0.0f)).fill(0.0f);

            float nc = arma::dot(computedC, computedC);
            computedA = A * (Ctmp * computedC) / (nc + std::numeric_limits<float>::epsilon());;
            computedA.elem(arma::find(computedA < 0.0f)).fill(0.0f);
        }

        float Atonorm = std::sqrt(arma::dot(computedA, computedA) + std::numeric_limits<float>::epsilon());
        computedA /= Atonorm;
        computedC *= Atonorm;

        ColumnFloat_t r = ((Ctmp.t() * (A.t() * computedA)) / (arma::dot(computedA, computedA) + std::numeric_limits<float>::epsilon())) - computedC;
        ColumnFloat_t inTrace = computedC + r;

        isx::constrainedFoopsi(inTrace, outArParams, outNoise, outCaTrace, outBaseline, outInitCaVal, outSpikes, inDeconvParams);

        outA = computedA;
        outYrA = inTrace - outCaTrace;
    }

    bool mergeComponents(
        MatrixFloat_t & inOutA,
        MatrixFloat_t & inOutC,
        MatrixFloat_t & inOutRawC,
        const float inCorrThresh,
        DeconvolutionParams inDeconvParams,
        const size_t inNumThreads)
    {
        const size_t K = inOutA.n_cols;  // number of cells
        const size_t d = inOutA.n_rows;  // number of pixels
        const size_t T = inOutC.n_cols;  // number of time points

        MatrixFloat_t Acorr = arma::trimatu(inOutA.t() * inOutA);
        Acorr.diag(0) = arma::zeros<ColumnFloat_t>(K);

        arma::umat ff2 = Acorr > 0.0f;
        MatrixFloat_t Ccorr = arma::zeros<MatrixFloat_t>(arma::size(Acorr));

        // Check correlation of calcium traces for all overlapping components
        for (size_t i = 0; i < K; ++i)
        {
            arma::uvec indices = arma::find(Acorr(arma::span(i), arma::span::all));
            for (size_t j = 0; j < indices.n_elem; ++j)
            {
                Ccorr(i, indices(j)) = pearsonr(inOutC.row(i).t(), inOutC.row(indices(j)).t());
            }
        }

        arma::umat ff1 = (Ccorr + Ccorr.t()) > inCorrThresh;
        arma::umat ff3 = ff1 % ff2;

        // Extract connected components
        uint32_t numComponents;
        arma::uvec connComponents;
        connectedComponents(ff3, numComponents, connComponents);

        arma::umat listConxcomp(K, 0);
        for (uint32_t idx = 0; idx < numComponents; ++idx)
        {
            if (arma::accu(connComponents == idx) > 1)
            {
                listConxcomp.insert_cols(listConxcomp.n_cols, (connComponents == idx));
            }
        }

        if (listConxcomp.n_elem == 0)
        {
//            ISX_LOG_INFO("No more components to merge");
            return false;
        }

        ColumnFloat_t cor = arma::zeros<ColumnFloat_t>(listConxcomp.n_cols);
        for (size_t idx = 0; idx < cor.n_elem; ++idx)
        {
            arma::uvec fm = arma::find(listConxcomp.col(idx));
            for (size_t jdx1 = 0; jdx1 < fm.n_elem; ++jdx1)
            {
                for (size_t jdx2 = jdx1 + 1; jdx2 < fm.n_elem; ++jdx2)
                {
                    cor(idx) = cor(idx) + Ccorr(fm(jdx1), fm(jdx2));
                }
            }
        }

        // Order to perform merges, based on correlation values
        const arma::uvec ind = cor.n_elem > 1 ? arma::reverse(arma::sort_index(cor)) : arma::uvec({0});

        // Number of merging operations
        size_t nbmrg = std::min(static_cast<size_t>(ind.n_elem), std::numeric_limits<size_t>::max());

        // Structures to store merged results
        MatrixFloat_t mergedA = arma::zeros<MatrixFloat_t>(d, nbmrg);
        MatrixFloat_t mergedC = arma::zeros<MatrixFloat_t>(T, nbmrg);
        MatrixFloat_t mergedRawC = arma::zeros<MatrixFloat_t>(T, nbmrg);

        arma::uvec mergedComponents; // Indices of components that undergo merging

        if (inNumThreads < 2)
        {
            // Merge components sequentially
            for (size_t idx = 0; idx < nbmrg; ++idx)
            {
                arma::uvec mergedRoi = arma::find(listConxcomp.col(ind(idx)));
                mergedComponents = arma::join_cols(mergedComponents, mergedRoi);

                ColumnFloat_t outCaTrace, outSpikes, outA, outYrA;
                float outBl, outC1;
                float outSn = -1.0f;
                std::vector<float> outArParams;

                mergeIteration(inOutA, inOutC, mergedRoi, outArParams, outSn, outCaTrace, outBl, outC1, outSpikes, outA, outYrA, inDeconvParams);

                mergedA.col(idx) = outA;
                mergedC.col(idx) = outCaTrace;
                mergedRawC.col(idx) = outYrA + outCaTrace;
            }

        }
        else
        {
            // Merge components in parallel
            ThreadPool pool(inNumThreads);

            std::vector<ColumnFloat_t> outCaTrace(nbmrg);
            std::vector<ColumnFloat_t> outSpikes(nbmrg);
            std::vector<ColumnFloat_t> outA(nbmrg);
            std::vector<ColumnFloat_t> outYrA(nbmrg);

            std::vector<float> outBl(nbmrg);
            std::vector<float> outC1(nbmrg);
            std::vector<float> outSn(nbmrg, -1.0f);
            std::vector<std::vector<float>> outArParams(nbmrg);

            std::vector<arma::uvec> mergedRoi(nbmrg);

            std::vector<std::future<void>> results(nbmrg);
            for (size_t idx = 0; idx < nbmrg; ++idx)
            {
                mergedRoi[idx] = arma::find(listConxcomp.col(ind(idx)));
                mergedComponents = arma::join_cols(mergedComponents, mergedRoi[idx]);

                results[idx] = pool.enqueue(
                    mergeIteration,
                    std::cref(inOutA),
                    std::cref(inOutC),
                    std::cref(mergedRoi[idx]),
                    std::ref(outArParams[idx]),
                    std::ref(outSn[idx]),
                    std::ref(outCaTrace[idx]),
                    std::ref(outBl[idx]),
                    std::ref(outC1[idx]),
                    std::ref(outSpikes[idx]),
                    std::ref(outA[idx]),
                    std::ref(outYrA[idx]),
                    inDeconvParams
                );
            }

            for (size_t idx = 0; idx < nbmrg; ++idx)
            {
                results[idx].get();
                mergedA.col(idx) = outA[idx];
                mergedC.col(idx) = outCaTrace[idx];
                mergedRawC.col(idx) = outYrA[idx] + outCaTrace[idx];
            }
        }

        // Remove components that were merged
        inOutA.shed_cols(mergedComponents);
        inOutC.shed_rows(mergedComponents);

        // Add merged results
        inOutA = arma::join_rows(inOutA, mergedA);
        inOutC = arma::join_cols(inOutC, mergedC.t());

        if (!inOutRawC.empty()) {
            inOutRawC.shed_rows(mergedComponents);
            inOutRawC = arma::join_cols(inOutRawC, mergedRawC.t());
        }

        return true;
    }
} // namespace isx
