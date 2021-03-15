#include "isxCnmfeMerging.h"
#include "isxTest.h"
#include "catch.hpp"


TEST_CASE("CnmfeMergeConnectedComponents", "[cnmfe-merging]")
{
    uint32_t actualNumComponents, expectedNumComponents;
    arma::uvec actualConnComponents, expectedConnComponents;

    SECTION("small synthetic data")
    {
        expectedNumComponents = 2;
        expectedConnComponents = {0,0,0,1,1};

        const arma::umat adjMatrix({
            {0,1,1,0,0},
            {0,0,1,0,0},
            {0,0,0,0,0},
            {0,0,0,0,1},
            {0,0,0,0,0}});

        isx::connectedComponents(adjMatrix, actualNumComponents, actualConnComponents);

        REQUIRE(approxEqual(expectedNumComponents, actualNumComponents, 0));
        REQUIRE(arma::approx_equal(expectedConnComponents, actualConnComponents, "reldiff", 0));
    }
}
