#include "isxCnmfeDeconv.h"
#include "isxCnmfeUtils.h"
#include "isxOasis.h"


namespace isx
{
    void constrainedFoopsi(
        const ColumnFloat_t & inTrace,
        std::vector<float> & inOutARParams,
        float & inOutNoise,
        ColumnFloat_t & outCaTrace,
        float & outBaseline,
        float & outInitCaVal,
        ColumnFloat_t & outSpikes,
        DeconvolutionParams inDeconvParams)
    {
        // estimate parameters if necessary
        if (inOutNoise < 0.0f)
        {
            inOutNoise = getNoiseFft(inTrace, inDeconvParams.m_noiseRange, inDeconvParams.m_noiseMethod);
        }
        if (inOutARParams.empty())
        {
            estimateTimeConst(inTrace, inOutNoise, inDeconvParams.m_lags, inDeconvParams.m_fudgeFactor, inDeconvParams.m_firstOrderAR, inOutARParams);
        }

        // use oasis algorithm for deconvolution
        // size_t p = inOutARParams.size();  // order of the AR model, we currently only support p=1 with OASIS
        Oasis oasis(inOutARParams[0], inOutNoise);
        oasis.solveFoopsi(inTrace, outBaseline, outInitCaVal, outCaTrace, outSpikes);
    }

    void estimateTimeConst(
        const ColumnFloat_t & inTrace,
        const float inNoise,
        const uint32_t inLags,
        const float inFudgeFactor,
        const bool inFirstOrderAR,
        std::vector<float> & outParams)
    {
        uint32_t lags = inLags + (inFirstOrderAR ? 1 : 2);

        ColumnFloat_t xc;
        autoCovariance(inTrace, xc, lags);

        MatrixFloat_t toeplitz = arma::toeplitz(xc(arma::span(lags, 2*lags - 1)),
                                                 xc(arma::span(lags, lags + (inFirstOrderAR ? 1 : 2) - 1)));
        MatrixFloat_t A = toeplitz - (inNoise * inNoise * arma::eye<MatrixFloat_t>(lags, (inFirstOrderAR ? 1 : 2)));

        ColumnFloat_t b = xc(arma::span(lags + 1, xc.n_elem - 1));

        ColumnFloat_t g = arma::solve(A, b);

        ColumnFloat_t polynomial = arma::join_cols(ColumnFloat_t({1.0f}), -ColumnFloat_t(g));
        ColumnFloat_t gRoots = arma::real(arma::roots(polynomial));

        // Add some variability to result - used saved values from np.random.normal with seed=45
        std::vector<float> random = {0.00026375f,  0.00260322f, -0.00395146f, -0.00204301f};
        auto randomIt = random.begin();

        gRoots.transform( [&](float val) {return (val > 1.0f) ? 0.95f + (*randomIt)++ : val; } );
        gRoots.transform( [&](float val) {return (val < 0.0f) ? 0.15f + (*randomIt)++ : val; } );
        gRoots *= inFudgeFactor;

        // Get AR Parameters by getting coefficients of polynomial with roots gRoots
        outParams.reserve((inFirstOrderAR ? 1 : 2));
        if (inFirstOrderAR)
        {
            outParams.emplace_back(gRoots(0));
        }
        else
        {
            outParams.emplace_back(gRoots(0) + gRoots(1));
            outParams.emplace_back(- gRoots(0) * gRoots(1));
        }
    }

} // namespace isx
