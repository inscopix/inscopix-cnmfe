#include "isxCnmfeUtils.h"
#include "isxTest.h"
#include "catch.hpp"


TEST_CASE("nextPowerOf2", "[cnmfe-utils]")
{
    REQUIRE(isx::nextPowerOf2(0) == 1);
    REQUIRE(isx::nextPowerOf2(1) == 1);
    REQUIRE(isx::nextPowerOf2(2) == 2);
    REQUIRE(isx::nextPowerOf2(1001) == 1024);
}

TEST_CASE("AutocovarianceTest", "[cnmfe-utils]")
{
    SECTION("max lag set to 0")
    {
        const isx::ColumnFloat_t input = {3.6f, 7.1f, 93.5f};
        const isx::ColumnFloat_t expResult = {1728.80222222f};

        isx::ColumnFloat_t actResult;
        isx::autoCovariance(input, actResult, 0);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("max lag of 1")
    {
        const isx::ColumnFloat_t input = {10.0f, 20.0f, 30.0f, 45.0f, 67.0f};
        const isx::ColumnFloat_t expResult = {142.728f, 399.44f , 142.728f};

        isx::ColumnFloat_t actResult;
        isx::autoCovariance(input, actResult, 1);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("small number of lags")
    {
        const isx::ColumnFloat_t input = {
            -0.55415571f,  0.13067809f, -0.47066189f,  0.18785275f, -0.81764995f,
            -0.26838182f,  1.19359672f,  0.26941436f, -1.88744652f,  0.301528f,
            -0.35005024f, -1.11426144f,  0.37029969f, -0.66274451f, -0.80652477f,
             1.27030236f, -1.31956239f,  0.1889118f,  -1.00103583f, -1.89837716f
        };

        const isx::ColumnFloat_t expResult = {
            0.05070829f, -0.25140394f,  0.23774302f, -0.11097512f, -0.20868557f,
            0.72200092f, -0.20868557f, -0.11097512f,  0.23774302f, -0.25140394f,
            0.05070829f
        };

        isx::ColumnFloat_t actResult;
        isx::autoCovariance(input, actResult, 5);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("large number of lags")
    {
        const isx::ColumnFloat_t input = {
             1.564001f, -0.32985317f, -0.10211276f,  1.10911775f,  2.93753495f,
            -0.03813587f,  0.02691793f, -1.39761765f, -0.45699528f,  2.13256227f,
             0.33569082f,  0.6203118f,  1.70275645f, -1.34475579f,  0.92696876f,
             0.62454571f, -0.5613353f,  0.84249566f,  1.02676119f,  1.08519391f,
             1.75884343f,  0.68893129f,  1.88028278f, -1.43297781f,  1.98428162f,
             0.75895819f,  2.12934656f, -1.03770882f,  0.60295206f, -1.20514837f,
             1.05198612f,  1.72823941f,  0.05110818f,  0.25447424f,  0.32344392f,
             0.11522589f,  1.51427686f,  0.45413881f,  0.79645454f, -0.35213528f,
             0.3118916f,  1.38134101f, -0.51227639f,  0.52379383f, -0.01489126f,
             0.09767708f, -0.14805649f,  0.32855176f, -0.30944516f,  1.31164424f
        };

        const isx::ColumnFloat_t expResult = {
            0.10690421f, -0.12533681f,  0.00395969f, -0.15443475f,  0.21799438f,
            0.10248488f, -0.06579026f, -0.22140925f,  0.01391488f, -0.21524513f,
            0.97459125f, -0.21524513f,  0.01391488f, -0.22140925f, -0.06579026f,
            0.10248488f,  0.21799438f, -0.15443475f,  0.00395969f, -0.12533681f,
            0.10690421f
        };

        isx::ColumnFloat_t actResult;
        isx::autoCovariance(input, actResult, 10);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }
}

TEST_CASE("CnmfeUtilsDecimationMatrix", "[cnmfe-utils]")
{
    SECTION("square matrix")
    {
        std::pair<int32_t,int32_t> inDims(3,3);
        int32_t subsamplingFactor = 2;

        arma::SpMat<float> actual = isx::generateDecimationMatrix(inDims, subsamplingFactor);

        const isx::MatrixFloat_t expected = {
            {0.25f, 0.25f, 0.0f  , 0.25f, 0.25f, 0.0f  , 0.0f  , 0.0f  , 0.0f},
            {0.0f  , 0.0f  , 0.5f , 0.0f  , 0.0f  , 0.5f , 0.0f  , 0.0f  , 0.0f},
            {0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.5f , 0.5f , 0.0f},
            {0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.0f  , 0.0f  , 1.0f}
        };

        REQUIRE(arma::approx_equal(isx::MatrixFloat_t(actual), expected, "reldiff", 1e-5f));
    }
}

TEST_CASE("CnmfeUtilsCentroid", "[cnmfe-utils]")
{
    SECTION("square matrix")
    {
        const isx::MatrixFloat_t inMatrix = {
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.2f, 1.8f, 0.0f},
            {0.0f, 1.1f, 1.9f, 0.0f},
            {0.0f, 1.5f, 1.3f, 0.0f}
        };
        std::pair<float, float> expectedCentroid(1.9772727272727268f, 1.5681818181818181f);

        std::pair<float, float> actualCentroid = isx::computeCentroid(inMatrix);

        REQUIRE(approxEqual(expectedCentroid.first, actualCentroid.first, 1e-5));
        REQUIRE(approxEqual(expectedCentroid.second, actualCentroid.second, 1e-5));
    }

    SECTION("non-square matrix")
    {
        const isx::MatrixFloat_t inMatrix = {
            {0.5f, 10.1f, 19.34f, 0.98f, 0.3f},
            {6.7f, 3.2f, 0.8f, 12.5f, 10.2f},
            {6.7f, 4.6f, 3.3f, 3.2f, 1.2f},
            {0.7f, 0.9f, 1.2f, 4.2f, 2.2f}
        };
        std::pair<float, float> expectedCentroid(1.0665804783451842f, 2.00732600732600731f);

        std::pair<float, float> actualCentroid = isx::computeCentroid(inMatrix);

        REQUIRE(approxEqual(expectedCentroid.first, actualCentroid.first, 1e-5));
        REQUIRE(approxEqual(expectedCentroid.second, actualCentroid.second, 1e-5));
    }

    SECTION("zero matrix")
    {
        const isx::MatrixFloat_t inMatrix = {
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
        };
        std::pair<float, float> expectedCentroid(1.0f, 2.5f);

        std::pair<float, float> actualCentroid = isx::computeCentroid(inMatrix);

        REQUIRE(approxEqual(expectedCentroid.first, actualCentroid.first, 1e-5));
        REQUIRE(approxEqual(expectedCentroid.second, actualCentroid.second, 1e-5));
    }
}

TEST_CASE("LocalCorrelationsFftTest", "[cnmfe-utils]")
{
    SECTION("3x3x3 input, 8 neighbours")
    {
        const float data[27] =
        {
            -0.07700959f, -0.11066488f, -0.0100138f,
            -0.79779015f,  0.11457479f, -0.65893541f,
            -0.10643261f,  0.765727f,  1.87412863f,

            -0.89436563f, -0.82184029f,  0.52091322f,
             0.33473636f,  0.24044337f, -0.9227193f,
            -0.85391f,  0.43819825f,  0.4628729f,

            -0.14107902f,  1.2408606f,  0.79075791f,
            -1.28539529f,  0.80208551f,  1.29718618f,
             0.20477652f,  0.62769642f, -0.54596479f
        };
        const isx::CubeFloat_t input(data, 3, 3, 3);

        const isx::MatrixFloat_t expResult = {
            { 0.02017717f, -0.84027034f,  0.1158743f },
            { 0.42596096f,  0.24681175f,  0.12510766f},
            { 0.6783232f,  0.41351622f, -0.3812494f }
        };

        isx::MatrixFloat_t actResult;
        isx::computeLocalCorr(input, actResult);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }

    SECTION("5x7x9 input, 8 neighbours")
    {
        const float data[315] =
        {
            -5.16531162e-01f, -1.30186955e+00f, -1.26431414e+00f,  1.02124922e+00f,
             1.22244724e-01f, -1.23336984e+00f, -7.67518107e-01f,  2.55533828e-01f,
             7.97866800e-01f,  4.29457335e-01f, -2.52741170e-01f,  1.63944159e+00f,
             1.12144378e+00f,  7.16060922e-01f, -1.52762121e-01f, -6.29008102e-01f,
             8.30810893e-01f, -1.72939619e-01f,  5.12212885e-01f, -2.64962967e+00f,
            -7.97995406e-01f, -1.01617168e-01f, -5.00216424e-01f,  1.96758861e+00f,
            -2.65112637e-02f, -5.39523537e-01f,  1.72334863e+00f, -1.06566900e+00f,
             1.46221388e+00f, -9.62476793e-01f,  1.51491796e+00f,  3.70726225e-01f,
             1.26912509e+00f, -1.46543254e+00f, -5.19484413e-02f, -8.01487326e-01f,
             1.75434748e-01f,  7.53916255e-01f,  3.90025785e-01f,  4.80546927e-02f,
            -7.30659458e-01f, -1.88839185e+00f,  9.02467460e-01f, -1.59249360e+00f,
            -3.29730344e-01f, -7.41028947e-02f, -3.49078741e-01f,  3.68194557e-01f,
            -1.21580484e+00f, -2.63561510e+00f, -1.06137594e+00f,  2.01552603e-01f,
            -2.91318597e-01f, -2.36405484e-01f,  3.56172657e-01f,  8.55343778e-01f,
             1.49794499e+00f, -1.56697712e-01f,  6.92632681e-01f, -1.54141225e+00f,
            -6.78352527e-01f, -2.18015762e-02f, -6.57111310e-01f, -1.91922995e-01f,
             5.03058306e-01f, -7.57468648e-01f,  2.52996162e-01f,  4.06971270e-02f,
            -4.35001480e-01f, -2.01486918e+00f,  5.43119717e-01f, -1.47119346e+00f,
            -1.44392007e-01f,  7.08552265e-01f,  2.42000535e-01f,  2.00742687e+00f,
             6.26791380e-01f,  1.51541579e+00f,  3.73678288e-01f,  6.55161212e-02f,
             6.85334557e-01f,  2.07371886e-01f, -3.14844055e-01f, -9.82643692e-01f,
            -3.25238097e-01f, -5.38365733e-01f, -4.65522331e-01f, -1.34679171e+00f,
            -1.07149171e+00f, -1.56385920e+00f,  6.74548814e-01f,  1.47994823e+00f,
             9.18386653e-01f, -8.41762935e-01f, -2.12144447e+00f,  2.00655026e-01f,
            -5.91415954e-01f, -1.45355885e+00f,  1.17437306e+00f, -9.70827453e-01f,
            -9.00596955e-01f, -9.68647554e-01f, -2.02821651e-01f, -1.23346876e+00f,
            -2.16863319e-01f, -9.37598868e-01f, -1.97978076e+00f, -5.75351024e-01f,
             7.43560522e-01f,  7.79813028e-01f, -1.19647856e+00f, -6.66952213e-01f,
             2.70294559e-01f, -8.40742836e-01f, -3.18420392e-01f,  1.21147275e+00f,
             5.99610065e-01f,  9.91259546e-02f, -2.38790240e-01f, -9.35643835e-02f,
            -6.63389680e-01f, -1.80053181e+00f,  9.34607748e-01f,  1.77392508e-01f,
            -1.19759425e+00f,  1.08870250e+00f,  7.38626412e-02f,  2.86040785e-01f,
             4.36934394e-03f,  1.35272401e+00f, -9.25845015e-01f,  3.15262484e-02f,
             1.34766191e-01f, -3.00198034e-01f, -2.02304166e+00f, -4.30733109e-01f,
             2.24050550e-01f,  6.33582337e-01f, -3.16696668e-01f, -3.84234319e-01f,
            -8.99191273e-01f,  1.36282707e-02f,  2.78110870e-01f,  3.50615148e-01f,
            -4.36188442e-02f,  5.88997982e-02f, -1.15102114e+00f, -7.54468844e-01f,
             1.25009076e+00f,  8.76038621e-01f,  4.43939287e-01f,  3.02593282e-02f,
             3.31290220e-03f, -5.70101477e-01f, -1.05561200e+00f,  4.16544945e-02f,
            -9.39544559e-01f, -2.62494644e-02f,  3.75526378e-02f, -2.36009433e+00f,
            -1.35396280e+00f,  9.32500187e-01f, -6.42544469e-01f, -8.33900511e-01f,
            -1.94480172e-02f,  4.93240808e-01f,  5.81484823e-01f,  3.47364031e-01f,
             1.13713079e+00f,  8.93713441e-01f,  3.85963486e-01f,  9.62271502e-01f,
             5.30031355e-01f,  3.03756007e-01f, -2.03372323e-01f,  1.94937680e+00f,
            -1.39048972e+00f, -1.21872863e+00f, -2.32832695e+00f, -9.22539028e-03f,
             2.20175702e-02f, -6.22604217e-01f, -8.47070547e-02f, -5.72726489e-01f,
            -1.23150857e+00f,  7.95510973e-01f, -1.57845591e+00f,  5.56176624e-01f,
             5.80061953e-01f,  4.38803485e-01f, -2.71695255e+00f, -3.74636168e-01f,
            -3.62958884e-01f, -1.12936383e+00f,  6.31804021e-01f, -1.52832442e+00f,
             1.43666051e-01f, -2.30007254e-01f,  4.94613131e-02f,  3.48549532e-01f,
             4.50005202e-02f, -6.87518367e-01f,  8.18502267e-01f,  4.29474123e-01f,
             1.78670023e+00f,  7.71568326e-01f,  4.39125411e-01f,  2.44382873e-01f,
             4.20002038e-02f, -1.61863211e+00f, -5.80371950e-01f, -3.40237782e-01f,
            -8.00850005e-01f, -2.30906267e-03f,  2.71544944e-01f,  5.36608656e-01f,
             1.01895748e+00f,  5.24502136e-01f, -7.99841122e-01f, -2.04797921e+00f,
            -9.61206098e-01f, -2.18715245e+00f,  6.09197891e-01f,  3.88549332e-01f,
            -1.19604287e+00f, -1.45532194e+00f,  6.50137409e-01f,  4.26074393e-01f,
             6.35683641e-01f, -3.32376587e-01f,  1.17036257e+00f, -6.26666819e-01f,
             8.77656762e-01f,  1.29244898e+00f,  3.50067531e-01f,  1.15713158e+00f,
            -7.60405112e-01f, -1.43757573e+00f, -3.51930509e-01f, -1.01368459e+00f,
            -6.02249993e-01f,  9.92387671e-01f, -8.21859106e-01f,  3.84121581e-01f,
             1.08662013e+00f,  1.74397333e-01f, -9.47920951e-02f, -9.46489884e-02f,
            -3.32254814e+00f, -3.63037910e+00f,  3.43370355e-01f, -1.02686682e+00f,
             4.95362262e-01f,  6.20569971e-01f,  1.09571652e+00f, -4.87238163e-01f,
             7.32548010e-01f, -2.24618511e-01f,  1.32236528e+00f,  6.28311433e-01f,
            -1.13587021e+00f,  1.37349113e+00f, -1.88010978e+00f, -1.46060918e+00f,
             1.42027864e+00f,  9.38644957e-02f, -1.83017554e-01f, -1.11707296e+00f,
            -5.57952335e-01f,  6.86436531e-01f, -8.16715538e-01f, -1.13546259e-01f,
            -8.58698995e-02f,  1.56987314e+00f, -1.52272781e+00f,  1.70228221e+00f,
            -2.35689389e-01f, -1.28705034e+00f,  1.19065642e+00f,  9.33462982e-01f,
            -5.76579158e-01f,  1.54158973e+00f,  8.70365617e-02f, -1.21085814e+00f,
            -1.71969872e+00f, -1.99458591e+00f,  6.59002119e-01f, -1.30041767e+00f,
             5.80932201e-02f,  4.74579434e-01f,  1.05119797e+00f, -1.24005596e-01f,
            -1.16960978e+00f,  5.41935469e-01f,  2.79776030e-01f, -1.88685530e+00f,
            -8.52453225e-01f,  6.90689957e-01f, -6.75587880e-01f, -1.35625678e+00f,
            -3.58684102e-01f, -3.23558893e-01f,  2.47264050e-01f, -4.30085424e-01f,
            -6.80536502e-02f,  1.95596673e+00f, -1.21670594e+00f, -3.60000372e-01f,
            -5.71376591e-01f, -6.31188439e-01f, -6.44242119e-01f,  1.14861179e+00f,
             5.23435919e-01f, -1.11576091e+00f,  1.07937272e+00f
        };
        const isx::CubeFloat_t input(data, 7, 9, 5);

        const isx::MatrixFloat_t expResult = {
            {-0.11949029f,  0.15791532f,  0.10774148f, -0.22715679f, -0.18931355f,
             -0.00995857f,  0.11702414f, -0.21127813f, -0.17452545f},
            { 0.01587946f, -0.05590378f, -0.15471916f, -0.21555667f, -0.0025323f,
              0.0557965f, -0.04699939f, -0.2766737f,  0.18496269f},
            {-0.14962003f, -0.19080296f,  0.1277571f,  0.02359892f,  0.32028967f,
              0.08173554f, -0.10184254f, -0.06537201f,  0.13672607f},
            {-0.02692715f, -0.00455937f, -0.44537538f,  0.19330917f,  0.50055534f,
              0.03770787f, -0.20744428f,  0.0153501f,  0.05708306f},
            {-0.21378317f, -0.02417896f,  0.08061232f,  0.20844987f, -0.13311017f,
              0.30068856f,  0.09797966f, -0.11858447f,  0.04402068f},
            {-0.3993176f,  0.05357676f, -0.22860137f, -0.00994436f,  0.09785801f,
              0.22504556f,  0.07691801f, -0.05931497f, -0.26694712f},
            {-0.24881859f, -0.2797123f,  0.11018972f,  0.2484267f,  0.23082519f,
              0.07116737f, -0.2764688f, -0.1598575f, -0.75758475f}
        };

        isx::MatrixFloat_t actResult;
        isx::computeLocalCorr(input, actResult);
        REQUIRE(arma::approx_equal(expResult, actResult, "reldiff", 1e-5f));
    }
}

TEST_CASE("LassoLars", "[cnmfe-utils]")
{
    SECTION("single predictor")
    {
        isx::MatrixFloat_t inX = {-0.04f, 0.6f, 2.4f, -1.0f, 0.9f, 1.7f, 0.0f};
        inX = inX.t();
        isx::RowFloat_t inY = {-1.0f,-2.0f,0.0f,3.0f,4.0f,9.0f,0.0f};
        isx::ColumnFloat_t expectedBeta = {0.6738859f};

        float lambda = 0.05f;
        isx::ColumnFloat_t outBeta;
        isx::lassoLars(inX, inY, outBeta, lambda, true);

        REQUIRE(arma::approx_equal(expectedBeta, outBeta, "reldiff", 1e-5f));
    }

    SECTION("multiple predictors")
    {
        isx::MatrixFloat_t inX = {
                {5.0f, 5.0f, 0.5f},
                {5.0f, 4.0f, 9.1f},
                {6.0f, 3.0f, 4.0f},
                {5.0f, 4.0f, 12.0f},
                {7.0f, -5.0f, 0.9f},
        };
        isx::RowFloat_t inY = {-1.0f, 2.0f, 42.0f, -6.0f, 1.0f};
        isx::ColumnFloat_t expectedBeta = {61.54273113f, 12.2913786f, 0.86923194f};

        float lambda = 0.02294787473418495f;
        isx::ColumnFloat_t outBeta;
        isx::lassoLars(inX, inY, outBeta, lambda, true);

        REQUIRE(arma::approx_equal(expectedBeta, outBeta, "reldiff", 1e-5f));
    }

    SECTION("negative coefficients")
    {
        isx::MatrixFloat_t inX = {0.0f, 1.0f, 1.0f};
        inX = inX.t();
        isx::RowFloat_t inY = {-1.0f,-2.0f,-3.0f};
        isx::ColumnFloat_t expectedBeta = {-0.39772962f};

        float lambda = 0.3f;
        isx::ColumnFloat_t outBeta;
        isx::lassoLars(inX, inY, outBeta, lambda, false);

        REQUIRE(arma::approx_equal(expectedBeta, outBeta, "reldiff", 1e-5f));
    }

    SECTION("positive constraint on coefficients")
    {
        isx::MatrixFloat_t inX = {0.0f, 1.0f, 1.0f};
        inX = inX.t();
        isx::RowFloat_t inY = {-1.0f,-2.0f,-3.0f};
        isx::ColumnFloat_t expectedBeta = {0.0f};

        float lambda = 0.3f;
        isx::ColumnFloat_t outBeta;
        isx::lassoLars(inX, inY, outBeta, lambda, true);

        REQUIRE(arma::approx_equal(expectedBeta, outBeta, "reldiff", 1e-5f));
    }
}

TEST_CASE("CnmfeUtilsScaleSpatialTemporalComponents")
{
    const size_t d1 = 3;
    const size_t d2 = 3;
    const size_t K = 2;

    isx::CubeFloat_t actA;
    {
        const float data[18] = {
            0.0f, 0.0f, 0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 0.0f, 0.0f,
        };
        actA = isx::CubeFloat_t(data, d1, d2, K);
    }
    
    isx::MatrixFloat_t actC = {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f},
    };

    isx::DeconvolutionParams deconvParams;

    SECTION("Normalized")
    {
        isx::scaleSpatialTemporalComponents(actA, actC, isx::CnmfeOutputType_t::DF);

        isx::CubeFloat_t expA;
        {
            const float data[18] = {
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.13483997f, 0.26967994f, 0.40451991f, 0.53935989f, 0.67419986f, 0.0f, 0.0f,
            };
            expA = isx::CubeFloat_t(data, d1, d2, K);
        }
        
        isx::MatrixFloat_t expC = {
            {100.0f, 200.0f, 300.0f, 400.0f, 500.0f},
            {7.41619848f, 14.83239697f, 22.24859546f, 29.66479394f, 37.08099243f},
        };

        REQUIRE(arma::approx_equal(actA, expA, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(actC, expC, "reldiff", 1e-5f));

        // Spatial footprints should be unit vectors (i.e., have a magnitude of one)
        isx::ColumnFloat_t actNorm(actA.n_slices);
        for (size_t k = 0; k < K; k++)
        {
            actNorm(k) = arma::norm(actA.slice(k), "fro");
        }

        isx::ColumnFloat_t expNorm(actA.n_slices);
        expNorm.fill(1.0f);

        REQUIRE(arma::approx_equal(actNorm, expNorm, "reldiff", 1e-5f));
    }

    SECTION("Noise scaled")
    {
        isx::scaleSpatialTemporalComponents(actA, actC, isx::CnmfeOutputType_t::NOISE_SCALED, deconvParams);

        isx::CubeFloat_t expA;
        {
            const float data[18] = {
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.13483997f, 0.26967994f, 0.40451991f, 0.53935989f, 0.67419986f, 0.0f, 0.0f,
            };
            expA = isx::CubeFloat_t(data, d1, d2, K);
        }
        
        isx::MatrixFloat_t expC = {
            {0.85065117f, 1.70130234f, 2.55195352f, 3.40260469f, 4.25325586f},
            {0.85065117f, 1.70130234f, 2.55195352f, 3.40260469f, 4.25325586f},
        };

        REQUIRE(arma::approx_equal(actA, expA, "reldiff", 1e-5f));
        REQUIRE(arma::approx_equal(actC, expC, "reldiff", 1e-5f));

        // Spatial footprints should be unit vectors (i.e., have a magnitude of one)
        isx::ColumnFloat_t actNorm(actA.n_slices);
        for (size_t k = 0; k < K; k++)
        {
            actNorm(k) = arma::norm(actA.slice(k), "fro");
        }

        isx::ColumnFloat_t expNorm(actA.n_slices);
        expNorm.fill(1.0f);

        REQUIRE(arma::approx_equal(actNorm, expNorm, "reldiff", 1e-5f));
    }
}
