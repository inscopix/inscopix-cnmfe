#include "catch.hpp"
#include "isxMemoryMappedFileUtils.h"
#include "isxCnmfeIO.h"

namespace {
    void convertMovieToCube(
        const isx::SpTiffMovie_t & inMovie,
        isx::CubeFloat_t & outCube)
    {
        outCube.set_size(inMovie->getFrameHeight(), inMovie->getFrameWidth(), inMovie->getNumFrames());
        for (size_t i = 0; i < inMovie->getNumFrames(); i++)
        {
            isx::MatrixFloat_t frame;
            inMovie->getFrame(i, frame);
            outCube.slice(i) = frame;
        }
    }
}

TEST_CASE("MemoryMapMovie", "[cnmfe-utils]")
{
    const std::string inputMoviePath = "test/data/movie_128x128x100.tif";
    const std::string outputMemoryMapPath = "test/data/mmap.bin";

    const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(inputMoviePath));
    const size_t numRows = movie->getFrameHeight();
    const size_t numCols = movie->getFrameWidth();
    const size_t numFrames = movie->getNumFrames();
    const isx::DataType dataType = movie->getDataType();
    
    isx::CubeFloat_t movieCube;
    convertMovieToCube(movie, movieCube);

    SECTION("Full FOV")
    {
        isx::writeMemoryMappedFileMovie(movie, outputMemoryMapPath);

        const std::tuple<size_t, size_t, size_t, size_t> roi = std::make_tuple(0, numRows - 1, 0, numCols - 1);
        isx::CubeFloat_t patch;
        isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, roi, patch);

        REQUIRE(arma::approx_equal(patch, movieCube, "reldiff", 1e-5f));
    }

    SECTION("Square patches")
    {
        isx::writeMemoryMappedFileMovie(movie, outputMemoryMapPath);

        // Patches used in CNMFE for FOV of size 128x128 pixels, with a patch size of 80 pixels and patch overlap of 20 pixels 
        std::vector<std::tuple<size_t,size_t,size_t,size_t>> rois = {
            std::make_tuple(0, 79, 0, 79),
            std::make_tuple(48, 127, 0, 79),
            std::make_tuple(0, 79, 48, 127),
            std::make_tuple(48, 127, 48, 127)
        };

        for (size_t i = 0; i < rois.size(); i++)
        {
            const isx::CubeFloat_t expectedPatch = movieCube(
                arma::span(std::get<0>(rois[i]), std::get<1>(rois[i])),
                arma::span(std::get<2>(rois[i]), std::get<3>(rois[i])),
                arma::span::all
            );

            isx::CubeFloat_t patch;
            isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, rois[i], patch);

            REQUIRE(arma::approx_equal(patch, expectedPatch, "reldiff", 1e-5f));
        }
    }

    SECTION("Rectangle patches")
    {
        isx::writeMemoryMappedFileMovie(movie, outputMemoryMapPath);

        std::vector<std::tuple<size_t,size_t,size_t,size_t>> rois = {
            std::make_tuple(0, 31, 0, 15),
            std::make_tuple(32, 63, 0, 15),
            std::make_tuple(0, 31, 16, 31),
            std::make_tuple(32, 63, 16, 31)
        };

        for (size_t i = 0; i < rois.size(); i++)
        {
            const isx::CubeFloat_t expectedPatch = movieCube(
                arma::span(std::get<0>(rois[i]), std::get<1>(rois[i])),
                arma::span(std::get<2>(rois[i]), std::get<3>(rois[i])),
                arma::span::all
            );

            isx::CubeFloat_t patch;
            isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, rois[i], patch);

            REQUIRE(arma::approx_equal(patch, expectedPatch, "reldiff", 1e-5f));
        }
    }

    std::remove(outputMemoryMapPath.c_str());
}

TEST_CASE("MemoryMapMovieInvalid", "[cnmfe-utils]")
{
    const std::string outputMemoryMapPath = "test/data/mmap.bin";
    const size_t numRows = 128;
    const size_t numCols = 128;
    const size_t numFrames = 100;
    const isx::DataType dataType = isx::DataType::F32;

    SECTION("ROI out of range of movie size")
    {
        isx::CubeFloat_t patch;
        std::tuple<size_t,size_t,size_t,size_t> roi = std::make_tuple(0, numRows, 0, numCols);
        REQUIRE_THROWS_WITH(
            isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, roi, patch),
            "Failed memory mapped file read. ROI out of range of input movie");
    }

    SECTION("ROI in incorrect form")
    {
        isx::CubeFloat_t patch;
        std::tuple<size_t,size_t,size_t,size_t> roi = std::make_tuple(numRows - 1, 0, numCols - 1, 0);
        REQUIRE_THROWS_WITH(
            isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, roi, patch),
            "Failed memory mapped file read. ROI is not in the correct form");
    }
}

TEST_CASE("MemoryMapMovieOddDimensions", "[cnmfe-utils]")
{
    const std::string inputMoviePath = "test/data/movie_25x19x100.tif";
    const std::string outputMemoryMapPath = "test/data/mmap.bin";
    
    // resize movie to have odd dimensions
    {
        const std::string originalMoviePath = "test/data/movie_128x128x100.tif";
        const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(originalMoviePath));
        isx::CubeFloat_t movieCube;
        convertMovieToCube(movie, movieCube);
        isx::saveFootprintsToTiffFile(movieCube(arma::span(0, 18), arma::span(0, 24), arma::span::all), inputMoviePath);
    }

    const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(inputMoviePath));
    const size_t numRows = movie->getFrameHeight();
    const size_t numCols = movie->getFrameWidth();
    const size_t numFrames = movie->getNumFrames();
    const isx::DataType dataType = movie->getDataType();
    
    isx::CubeFloat_t movieCube;
    convertMovieToCube(movie, movieCube);

    SECTION("Full FOV")
    {
        isx::writeMemoryMappedFileMovie(movie, outputMemoryMapPath);

        const std::tuple<size_t, size_t, size_t, size_t> roi = std::make_tuple(0, numRows - 1, 0, numCols - 1);
        isx::CubeFloat_t patch;
        isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, roi, patch);

        REQUIRE(arma::approx_equal(patch, movieCube, "reldiff", 1e-5f));
    }

    SECTION("Patches")
    {
        isx::writeMemoryMappedFileMovie(movie, outputMemoryMapPath);

        std::vector<std::tuple<size_t,size_t,size_t,size_t>> rois = {
            std::make_tuple(0, 6, 0, 12),
            std::make_tuple(0, 17, 10, 24),
        };

        for (size_t i = 0; i < rois.size(); i++)
        {
            const isx::CubeFloat_t expectedPatch = movieCube(
                arma::span(std::get<0>(rois[i]), std::get<1>(rois[i])),
                arma::span(std::get<2>(rois[i]), std::get<3>(rois[i])),
                arma::span::all
            );

            isx::CubeFloat_t patch;
            isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, rois[i], patch);

            REQUIRE(arma::approx_equal(patch, expectedPatch, "reldiff", 1e-5f));
        }
    }

    std::remove(inputMoviePath.c_str());
    std::remove(outputMemoryMapPath.c_str());
}
