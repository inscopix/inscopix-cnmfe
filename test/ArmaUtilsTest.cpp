#include "isxArmaUtils.h"
#include "catch.hpp"


TEST_CASE("matrixToCubeByCol", "[arma-utils]")
{
    const isx::MatrixFloat_t matrix = {
        { 2.62171872f, 3.39921376f, 4.04735868f, 5.77486163f },
        { 5.59943288f, 5.38861832f, 4.87952942f, 5.59685709f },
        { 5.28988412f, 3.99440801f, 3.22557416f, 3.56880412f },
        { 5.1718189f, 3.39921376f, 4.87952942f, 5.77486163f },
        { 5.59685709f, 3.39921376f, 5.1718189f, 3.39921376f },
        { 8.59685709f, 1.51515161f, 6.1917001f, 2.19471919f },
    };

    const size_t numRows = 2;
    const size_t numCols = 3;

    isx::CubeFloat_t expectedCube(numRows, numCols, matrix.n_cols);
    expectedCube.slice(0) = {
        {2.62171872f, 5.28988412f, 5.59685709f},
        {5.59943288f, 5.1718189f, 8.59685709f}
    };
    expectedCube.slice(1) = {
        {3.39921376f, 3.99440801f, 3.39921376f},
        {5.38861832f, 3.39921376f, 1.51515161f}
    };
    expectedCube.slice(2) = {
        {4.04735868f, 3.22557416f, 5.1718189f},
        {4.87952942f, 4.87952942f, 6.1917001f}
    };
     expectedCube.slice(3) = {
        {5.77486163f, 3.56880412f, 3.39921376f},
        {5.59685709f, 5.77486163f, 2.19471919f}
    };

    isx::CubeFloat_t actualCube = isx::matrixToCubeByCol(matrix, numRows, numCols);

    REQUIRE(arma::approx_equal(actualCube, expectedCube, "reldiff", 1e-5f));
}

TEST_CASE("cubeToMatrixBySlice", "[arma-utils]")
{
    isx::CubeFloat_t cube(2, 3, 4);
    cube.slice(0) = {
        {2.62171872f, 5.28988412f, 5.59685709f},
        {5.59943288f, 5.1718189f, 8.59685709f}
    };
    cube.slice(1) = {
        {3.39921376f, 3.99440801f, 3.39921376f},
        {5.38861832f, 3.39921376f, 1.51515161f}
    };
    cube.slice(2) = {
        {4.04735868f, 3.22557416f, 5.1718189f},
        {4.87952942f, 4.87952942f, 6.1917001f}
    };
    cube.slice(3) = {
        {5.77486163f, 3.56880412f, 3.39921376f},
        {5.59685709f, 5.77486163f, 2.19471919f}
    };

    SECTION("column-major ordering")
    {
        const bool colOrder = true;
        const isx::MatrixFloat_t expectedMatrix = {
            { 2.62171872f, 3.39921376f, 4.04735868f, 5.77486163f },
            { 5.59943288f, 5.38861832f, 4.87952942f, 5.59685709f },
            { 5.28988412f, 3.99440801f, 3.22557416f, 3.56880412f },
            { 5.1718189f, 3.39921376f, 4.87952942f, 5.77486163f },
            { 5.59685709f, 3.39921376f, 5.1718189f, 3.39921376f },
            { 8.59685709f, 1.51515161f, 6.1917001f, 2.19471919f },
        };

        isx::MatrixFloat_t actualMatrix = isx::cubeToMatrixBySlice(cube, colOrder);

        REQUIRE(arma::approx_equal(actualMatrix, expectedMatrix, "reldiff", 1e-5f));
    }

    SECTION("row-major ordering")
    {
        const bool colOrder = false;
        const isx::MatrixFloat_t expectedMatrix = {
            { 2.62171872f, 3.39921376f, 4.04735868f, 5.77486163f },
            { 5.28988412f, 3.99440801f, 3.22557416f, 3.56880412f },
            { 5.59685709f, 3.39921376f, 5.1718189f, 3.39921376f },
            { 5.59943288f, 5.38861832f, 4.87952942f, 5.59685709f },
            { 5.1718189f, 3.39921376f, 4.87952942f, 5.77486163f },
            { 8.59685709f, 1.51515161f, 6.1917001f, 2.19471919f },
        };

        isx::MatrixFloat_t actualMatrix = isx::cubeToMatrixBySlice(cube, colOrder);

        REQUIRE(arma::approx_equal(actualMatrix, expectedMatrix, "reldiff", 1e-5f));
    }
}
