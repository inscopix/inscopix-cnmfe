#include "isxCnmfeGreedy.h"
#include "isxCnmfeTemporal.h"
#include "isxCnmfeUtils.h"
#include "isxTest.h"

#include "catch.hpp"
#include <thread>

TEST_CASE("CnmfeDownscale", "[cnmfe-greedycorr]") {

    SECTION("4x4 Downscale by 2x2")
    {
        const isx::MatrixFloat_t input = {
            { 1.0f,  2.0f,  3.0f,  4.0f},
            { 5.0f,  6.0f,  7.0f,  8.0f},
            { 9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        const isx::MatrixFloat_t actOutput = isx::downscale(input, {2, 2});

        const isx::MatrixFloat_t expOutput = {
            { 3.5f,  5.5f},
            {11.5f, 13.5f}
        };
        REQUIRE(arma::approx_equal(actOutput, expOutput, "reldiff", 1e-5f));
    }

    SECTION("4x4 Downscale by 3x3")
    {
        const isx::MatrixFloat_t input = {
            { 1.0f,  2.0f,  3.0f,  4.0f},
            { 5.0f,  6.0f,  7.0f,  8.0f},
            { 9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        const isx::MatrixFloat_t actOutput = isx::downscale(input, {3, 3});

        const isx::MatrixFloat_t expOutput = {
            { 6.0f,  8.0f},
            {14.0f, 16.0f}
        };
        REQUIRE(arma::approx_equal(actOutput, expOutput, "reldiff", 1e-5f));
    }

    SECTION("6x5 Downscale by 2x3")
    {
        const isx::MatrixFloat_t input = {
            {0.51878782f, 0.51561983f, 0.04401809f, 0.08010628f, 0.39137081f},
            {0.56440738f, 0.80331857f, 0.11279907f, 0.23413214f, 0.12467344f},
            {0.84737912f, 0.01705735f, 0.78494474f, 0.49383711f, 0.56010797f},
            {0.84321901f, 0.02070758f, 0.3593029f, 0.35530767f, 0.98046429f},
            {0.74324232f, 0.23066671f, 0.2333539f, 0.7104991f, 0.1528215f },
            {0.35011488f, 0.05655386f, 0.1352604f, 0.28736528f, 0.9192177f }
        };

        const isx::MatrixFloat_t actOutput = isx::downscale(input, {2, 3});

        const isx::MatrixFloat_t expOutput = {
            {0.42649179f, 0.20757067f},
            {0.47876845f, 0.59742926f},
            {0.29153201f, 0.51747589f}
        };
        REQUIRE(arma::approx_equal(actOutput, expOutput, "reldiff", 1e-5f));
    }

    SECTION("3x5 Downscale by 2x3")
    {
        const isx::MatrixFloat_t input = {
            {0.11332503f, 0.10653042f, 0.19008664f, 0.33285312f, 0.0648964f },
            {0.71527437f, 0.70422028f, 0.936895f, 0.72603783f, 0.11164153f},
            {0.09597526f, 0.11368147f, 0.47008167f, 0.1001078f, 0.6684117f }
        };

        const isx::MatrixFloat_t actOutput = isx::downscale(input, {2, 2});

        const isx::MatrixFloat_t expOutput = {
            {0.40983752f, 0.54646815f, 0.08826897f},
            {0.10482836f, 0.28509473f, 0.6684117f }
        };
        REQUIRE(arma::approx_equal(actOutput, expOutput, "reldiff", 1e-5f));
    }
}

TEST_CASE("CnmfeComputeB", "[cnmfe-greedycorr]") {

    SECTION("4x4x2 Movie Downscale by 2x2")
    {
        isx::MatrixFloat_t b0 = {
            {0.15058761f, 0.11791388f, 0.24581327f, 0.06779427f},
            {0.83080177f, 0.4939764f, 0.59475213f, 0.80717689f},
            {0.99167416f, 0.01185494f, 0.71433943f, 0.98855974f},
            {0.32638166f, 0.09801123f, 0.78682493f, 0.89621072f}
        };

        isx::MatrixFloat_t W = {
            {0.62191049f, 0.37154617f, 0.44277257f, 0.41153362f},
            {0.80670539f, 0.05332758f, 0.69106056f, 0.86300882f},
            {0.69797408f, 0.61875589f, 0.02480935f, 0.63050177f},
            {0.91668704f, 0.79181525f, 0.15062891f, 0.29773425f}
        };

        isx::CubeFloat_t B(4, 4, 2);
        B.slice(0) = {
            {0.89928363f, 0.78002872f, 0.80652918f, 0.4212873f },
            {0.16570465f, 0.57291088f, 0.65686042f, 0.32007659f},
            {0.83817957f, 0.61807914f, 0.72635293f, 0.10539732f},
            {0.18695031f, 0.79765369f, 0.81960148f, 0.32288851f}
        };
        B.slice(1) = {
            {0.55375682f, 0.95691201f, 0.09611342f, 0.94260467f},
            {0.54786219f, 0.80002615f, 0.80311126f, 0.42939133f},
            {0.79769035f, 0.46626885f, 0.35730661f, 0.4509553f },
            {0.29065614f, 0.91493919f, 0.34960848f, 0.01740093f}
        };

        isx::CubeFloat_t expOutput(4, 4, 2);
        expOutput.slice(0) = {
            {-0.28180353f, -0.2491298f, -0.32691511f, -0.1488961f },
            {-0.96201769f, -0.62519232f, -0.67585397f, -0.88827872f},
            {-0.95143403f,  0.02838519f, -1.01718609f, -1.2914064f },
            {-0.28614153f, -0.05777111f, -1.08967159f, -1.19905738f}
        };
        expOutput.slice(1) = {
            {-0.27813343f, -0.2454597f, -0.28271479f, -0.10469578f},
            {-0.95834759f, -0.62152222f, -0.63165365f, -0.84407841f},
            {-0.87978495f,  0.10003426f, -1.0668784f, -1.34109871f},
            {-0.21449246f,  0.01387797f, -1.1393639f, -1.24874969f}
        };

        isx::computeB(b0, arma::SpMat<float>(W), B, 2);
        REQUIRE(arma::approx_equal(arma::vectorise(B), arma::vectorise(expOutput), "reldiff", 1e-5f));
    }

    SECTION("5x3x2 Movie Downscale by 2x2")
    {
        isx::MatrixFloat_t b0 = {
            {0.52189876f, 0.65756561f, 0.36466804f, 0.28221182f, 0.75290584f},
            {0.8150039f, 0.51163538f, 0.81316376f, 0.6756913f, 0.81369033f},
            {0.81568421f, 0.54703491f, 0.88847746f, 0.55341325f, 0.96761203f}
        };

        isx::MatrixFloat_t W = {
            {0.97477428f, 0.53695014f, 0.27967224f, 0.03858401f, 0.27611478f, 0.87043108f},
            {0.25705418f, 0.53134937f, 0.99775839f, 0.77322357f, 0.69202304f, 0.2267709f },
            {0.99938942f, 0.2735458f, 0.41614151f, 0.28892286f, 0.60209485f, 0.5934592f },
            {0.08979304f, 0.72999046f, 0.7461985f, 0.22603311f, 0.36146908f, 0.49836779f},
            {0.97891188f, 0.52818393f, 0.31734023f, 0.59275061f, 0.56851618f, 0.91828338f},
            {0.10014738f, 0.09327738f, 0.95828616f, 0.06878422f, 0.71782334f, 0.61614039f}
        };

        isx::CubeFloat_t B(3, 5, 2);
        B.slice(0) = {
            {0.13340967f, 0.64247083f, 0.95298257f, 0.04027744f, 0.5222565f },
            {0.57135541f, 0.80522079f, 0.14098399f, 0.70328758f, 0.4434766f },
            {0.30151109f, 0.02292577f, 0.56650648f, 0.32964023f, 0.6163158f }
        };
        B.slice(1) = {
            {0.61670781f, 0.2655198f, 0.25325256f, 0.37610973f, 0.94699761f},
            {0.13850813f, 0.16422681f, 0.83247951f, 0.95495307f, 0.64984428f},
            {0.81908871f, 0.88059027f, 0.7401445f, 0.59344409f, 0.14309717f}
        };

        isx::CubeFloat_t expOutput(3, 5, 2);
        expOutput.slice(0) = {
            { 0.26314667f,  0.12747982f,  0.36492951f,  0.44738572f,  0.28663616f},
            {-0.02995847f,  0.27341005f, -0.08356622f,  0.05390625f,  0.22585167f},
            { 0.0558318f,  0.32448109f, -0.10059137f,  0.23447284f, -0.38801881f}
        };
        expOutput.slice(1) = {
            { 0.40553542f,  0.26986857f,  0.38594443f,  0.46840064f,  0.23976682f},
            { 0.11243028f,  0.4157988f, -0.0625513f,  0.07492117f,  0.17898232f},
            {-0.6720307f, -0.40338141f, -0.61655538f, -0.28149116f, -0.51669805f}
        };

        isx::computeB(b0, arma::SpMat<float>(W), B, 2);
        REQUIRE(arma::approx_equal(arma::vectorise(B), arma::vectorise(expOutput), "reldiff", 1e-5f));
    }
}
