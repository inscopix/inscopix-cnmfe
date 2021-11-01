#include "isxTiffMovie.h"
#include "catch.hpp"

TEST_CASE("TiffMovie-GetF32Frame", "[tiff-movie]")
{
    // Synthetic movie with regularly spaced element values
    const std::string filename = "test/data/synthetic_movie_float32.tif";  // movie dims: 3x4x5 (width * height * num_frames)
    isx::TiffMovie movie(filename);

    size_t numRows = 4;
    size_t numCols = 3;
    size_t numFrames = 5;

    SECTION("As F32 frame")
    {
        arma::Col<float> data = arma::regspace<arma::Col<float>>(0, 1, numRows * numCols * numFrames);
        arma::Cube<float> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<float> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::approx_equal(frame, expectedCube.slice(i), "reldiff", 1e-5));
        }
    }

    SECTION("As U16 frame")
    {
        arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
        arma::Cube<uint16_t> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<uint16_t> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::all(arma::vectorise(frame == expectedCube.slice(i))));
        }
    }
}

TEST_CASE("TiffMovie-GetU16Frame", "[tiff-movie]")
{
    // Synthetic movie with regularly spaced element values
    const std::string filename = "test/data/synthetic_movie_uint16.tif"; // movie dims: 3x4x5 (width * height * num_frames)
    isx::TiffMovie movie(filename);

    size_t numRows = 4;
    size_t numCols = 3;
    size_t numFrames = 5;

    arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
    arma::Cube<uint16_t> expectedCube(data.memptr(), numRows, numCols, numFrames);

    SECTION("As F32 frame")
    {
        arma::Col<float> data = arma::regspace<arma::Col<float>>(0, 1, numRows * numCols * numFrames);
        arma::Cube<float> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<float> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::approx_equal(frame, expectedCube.slice(i), "reldiff", 1e-5));
        }
    }

    SECTION("As U16 frame")
    {
        arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
        arma::Cube<uint16_t> expectedCube(data.memptr(), numRows, numCols, numFrames);

        for (size_t i = 0; i < numFrames; i++)
        {
            arma::Mat<uint16_t> frame;
            movie.getFrame(i, frame);
            REQUIRE(arma::all(arma::vectorise(frame == expectedCube.slice(i))));
        }
    }
}
