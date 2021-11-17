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

    /// Helper function to convert a single raw trace into the corresponding deconvolved trace and inferred spikes
    ///
    /// \param inRawC           Column vector containing the raw fluorescence trace
    /// \param outC             Column vector containing the deconvolved fluorescence trace
    /// \param outS             Discretized deconvolved neural activity (spikes)
    /// \param outSn            Standard deviation of the noise distribution. Estimated if a negative value is provided
    /// \param inDeconvParams   Parameters for estimating noise and autoregressive model used for deconvolution
    void deconvolveSingleTrace(
        const ColumnFloat_t & inRawC,
        ColumnFloat_t & outC,
        ColumnFloat_t & outS,
        float & outSn,
        DeconvolutionParams inDeconvParams)
    {
        outC = inRawC;
        outS.clear();
        outS.copy_size(inRawC);
        outSn = -1;

        float initCaVal, baseline;
        std::vector<float> arParams;
        isx::constrainedFoopsi(inRawC, arParams, outSn, outC, baseline, initCaVal, outS, inDeconvParams);
    }

    void deconvolveTraces(
        const MatrixFloat_t & inRawC,
        MatrixFloat_t & outC,
        MatrixFloat_t & outS,
        ColumnFloat_t & outSn,
        DeconvolutionParams inDeconvParams,
        const size_t inNumIterations)
    {
        const size_t K = inRawC.n_rows;
        outC = inRawC;

        outS.clear();
        outS.copy_size(inRawC);
        outSn.set_size(inRawC.n_rows);

        for (size_t i = 0; i < inNumIterations; i++)
        {
            for (size_t k = 0; k < K; k++)
            {
                isx::ColumnFloat_t c, s;
                float ca1, b;
                float sn = -1;
                std::vector<float> arParams;
                isx::ColumnFloat_t y = outC.row(k).t();
                isx::constrainedFoopsi(y, arParams, sn, c, b, ca1, s, inDeconvParams);
                outC.row(k) = c.t();
                outS.row(k) = s.t();
                outSn(k) = sn;

                ColumnFloat_t tmpC, tmpS;
                float tmpSn = -1;
                deconvolveSingleTrace(y, tmpC, tmpS, tmpSn, inDeconvParams);
            }
        }
    }

} // namespace isx
