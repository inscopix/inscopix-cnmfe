#include "isxTiffMovie.h"
#include "isxTest.h"
#include "catch.hpp"

TEST_CASE("TiffMovie-GetF32Frame", "[tiff-movie]")
{
    const size_t numRows = 4;
    const size_t numCols = 3;
    const size_t numFrames = 5;

    // Create a synthetic movie with regularly spaced element values
    const std::string filename = "test/data/tmp_synthetic_movie_float32.tif";  // movie dims: 3x4x5 (width * height * num_frames)
    {
        const arma::Col<float> data = arma::regspace<arma::Col<float>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<float> cube(data.memptr(), numRows, numCols, numFrames);

        saveCubeToTiffFile(cube, filename);
    }

    isx::TiffMovie movie(filename);

    SECTION("As F32 frame")
    {
        const arma::Col<float> data = arma::regspace<arma::Col<float>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<float> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<float> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::approx_equal(frame, expectedCube.slice(i), "reldiff", 1e-5));
        }
    }

    SECTION("As U16 frame")
    {
        const arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<uint16_t> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<uint16_t> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::approx_equal(frame, expectedCube.slice(i), "abs_tol", 0));
        }
    }

    std::remove(filename.c_str());
}

TEST_CASE("TiffMovie-GetU16Frame", "[tiff-movie]")
{
    const size_t numRows = 4;
    const size_t numCols = 3;
    const size_t numFrames = 5;

    // Create a synthetic movie with regularly spaced element values
    const std::string filename = "test/data/tmp_synthetic_movie_uint16.tif"; // movie dims: 3x4x5 (width * height * num_frames)
    {
        const arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<uint16_t> cube(data.memptr(), numRows, numCols, numFrames);

        saveCubeToTiffFile(cube, filename);
    }

    isx::TiffMovie movie(filename);

    SECTION("As F32 frame")
    {
        const arma::Col<float> data = arma::regspace<arma::Col<float>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<float> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<float> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::approx_equal(frame, expectedCube.slice(i), "reldiff", 1e-5));
        }
    }

    SECTION("As U16 frame")
    {
        const arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<uint16_t> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<uint16_t> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::approx_equal(frame, expectedCube.slice(i), "abs_tol", 0));
        }
    }

    std::remove(filename.c_str());
}
