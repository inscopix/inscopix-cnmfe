#include "isxOasis.h"
#include "isxArmaUtils.h"
#include "isxTest.h"
#include "catch.hpp"

TEST_CASE("OasisAccuracy", "[oasis]")
{
    SECTION("48 time points")
    {
        const isx::ColumnFloat_t y = {
          3.32591956e+00f, 7.67082838e+00f, 9.70239942e+00f, 5.36445782e+00f,
          3.00189889e+00f, 4.71801982e+00f, 3.94407202e+00f, 7.72786012e+00f,
          1.34549438e-04f, 5.91003054e+00f, 8.67105735e+00f, 4.29735426e+00f,
          4.93791089e+00f, 6.90551691e+00f, 7.33869010e-01f, 2.62360652e+00f,
          6.58549708e+00f, 7.63917213e+00f, 9.09395799e+00f, 6.87272154e+00f,
          6.64046418e+00f, 6.00471101e+00f, 1.59646058e+00f, 5.32007537e-01f,
          6.25557479e+00f, 5.96299013e+00f, 8.99545910e+00f, 5.90384334e+00f,
          2.90766717e+00f, 2.17656989e+00f, 3.70074120e+00f, 2.60021217e+00f,
          5.51730974e+00f, 7.00694137e+00f, 7.84384219e+00f, 6.99688739e-01f,
          8.56769056e-01f, 7.01963256e-01f, 1.66996248e+00f, 6.09312143e+00f,
          3.48657027e+00f, 6.14775144e-01f, 3.69198661e+00f, 9.28780428e+00f,
          4.94207676e+00f, 6.72699164e+00f, 1.85441586e+00f, 3.10812260e+00f
      };
      const float gamma = 0.7f;
      const float noise = 1.5f;

      const isx::ColumnFloat_t expectedC = {
        0.0f, 4.9018264f, 5.85887f, 4.1012087f, 2.870846f,
        2.9364889f, 2.2561424f, 3.3570228f, 2.349916f, 4.365591f,
        6.109313f, 4.276519f, 3.442689f, 3.1532338f, 2.2072637f,
        1.5450845f, 5.1098003f, 6.165326f, 7.6214075f, 5.401078f,
        4.374759f, 3.0623314f, 2.143632f, 1.5005424f, 4.7856917f,
        4.493214f, 6.2653728f, 4.385761f, 3.0700326f, 2.1490226f,
        2.0286117f, 1.420028f, 4.047762f, 4.789611f, 3.3527277f,
        2.3469095f, 1.6428366f, 1.1499856f, 0.80498993f, 3.2464557f,
        2.2725189f, 1.5907633f, 2.2224588f, 6.8785644f, 4.814995f,
        3.4648178f, 2.4253724f, 1.6977606f
      };
      const isx::ColumnFloat_t expectedS = {
        0.0f, 4.9018264f, 2.4275908f, 0.0f, 0.0f,
        0.9268961f, 0.20059991f, 1.7777232f, 0.0f, 2.7206497f,
        3.0533986f, 0.0f, 0.44912553f, 0.74335146f, 0.0f,
        0.0f, 4.028241f, 2.588466f, 3.3056793f, 0.06609297f,
        0.59400463f, 0.0f, 0.0f, 0.0f, 3.7353117f,
        1.1432297f, 3.1201222f, 0.0f, 0.0f, 0.0f,
        0.52429557f, 0.0f, 3.053742f, 1.956178f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 2.6829622f,
        0.0f, 0.0f, 1.1089239f, 5.3228436f, 0.0f,
        0.09432125f, 0.0f, 0.0f
      };
      const float expectedB = 1.1762477f;
      const float expectedCa1 = 1.856392f;

      isx::ColumnFloat_t actualC, actualS;
      float actualB, actualCa1;

      isx::Oasis oasis(gamma, noise);
      oasis.solveFoopsi(y, actualB, actualCa1, actualC, actualS);

      REQUIRE(approxEqual(actualB, expectedB, 1e-5));
      REQUIRE(approxEqual(actualCa1, expectedCa1, 1e-5));
      // REQUIRE(approxEqual(arma::norm_dot(actualC, expectedC), 1.0, 1e-7));
      // REQUIRE(approxEqual(arma::norm_dot(actualS, expectedS), 1.0, 1e-7));
    }
}
