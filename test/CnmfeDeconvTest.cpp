#include "isxCnmfeDeconv.h"
#include "isxTest.h"
#include "catch.hpp"

TEST_CASE("CnmfeDeconvEstimateTimeConst", "[cnmfe-deconv]")
{
    const isx::ColumnFloat_t inTrace = {
        15.9087785f, 39.093574f, 37.0103463f, 63.2176322f, 25.0918832f,
        52.0427554f, 52.2357718f, 68.6220029f, 48.9686648f, 29.5139094f
    };

    const float inNoise = 5.0f;
    const uint32_t lags = 5;
    const float inFudgeFactor = 1.0f;

    std::vector<float> outARParams;

    SECTION("Second Order AR")
    {
        const float inFirstOrderAR = false;

        isx::estimateTimeConst(inTrace, inNoise, lags, inFudgeFactor, inFirstOrderAR, outARParams);

        const std::vector<float> expArParams = {0.235851f, -0.0128607f};

        REQUIRE(approxEqual(outARParams[0], expArParams[0], 1e-5));
        REQUIRE(approxEqual(outARParams[1], expArParams[1], 1e-5));
    }

    SECTION("First Order AR")
    {
        const float inFirstOrderAR = true;

        isx::estimateTimeConst(inTrace, inNoise, lags, inFudgeFactor, inFirstOrderAR, outARParams);

        const std::vector<float> expArParams = {0.150264f};

        REQUIRE(approxEqual(outARParams[0], expArParams[0], 1e-5));
    }
}
