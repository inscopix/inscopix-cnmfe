#include "catch.hpp"
#include "isxMemoryMappedFileUtils.h"
#include "isxCnmfeIO.h"
#include "isxTest.h"

#include <QFile>

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
    const std::string inputMoviePath = "test/data/movie.tif";  // movie dims: 128x128x100 (width * height * num_frames)
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
            "Failed memory mapped file read. Roi(0, 128, 0, 128) out of range of input movie.");
    }

    SECTION("ROI range is non-increasing")
    {
        // The bounds defined for both rows and columns of a region of interest must be listed in increasing order
        // For instance here the ROI is incorrectly formatted like so: (end row index, start row index, end col index, start col index)
        // However the correct format of an ROI is: (start row index, end row index, start col index, end col index)
        isx::CubeFloat_t patch;
        std::tuple<size_t,size_t,size_t,size_t> roi = std::make_tuple(numRows - 1, 0, numCols - 1, 0);
        REQUIRE_THROWS_WITH(
            isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, roi, patch),
            "Failed memory mapped file read. Roi(127, 0, 127, 0) range is non-increasing.");
    }
}

TEST_CASE("MemoryMapMovieOddDimensions", "[cnmfe-utils]")
{
    const std::string inputMoviePath = "test/data/movie_25x19x100.tif";
    const std::string outputMemoryMapPath = "test/data/mmap.bin";
    
    // resize movie to have odd dimensions
    {
        const std::string originalMoviePath = "test/data/movie.tif";  // movie dims: 128x128x100 (width * height * num_frames)
        const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(originalMoviePath));
        isx::CubeFloat_t movieCube;
        convertMovieToCube(movie, movieCube);
        isx::saveFootprintsToTiffFile(movieCube(arma::span(0, 18), arma::span(0, 24), arma::span::all), inputMoviePath);
    }

    isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(inputMoviePath));
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

    movie.reset();
    std::remove(inputMoviePath.c_str());
    std::remove(outputMemoryMapPath.c_str());
}

TEST_CASE("MemoryMapMovieDeleteFile", "[cnmfe-utils]")
{
    const std::string inputMoviePath = "test/data/movie.tif";  // movie dims: 128x128x100 (width * height * num_frames)
    const std::string outputMemoryMapPath = "test/data/mmap.bin";

    const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(inputMoviePath));
    const size_t numRows = movie->getFrameHeight();
    const size_t numCols = movie->getFrameWidth();
    const size_t numFrames = movie->getNumFrames();
    const isx::DataType dataType = movie->getDataType();
    
    isx::CubeFloat_t movieCube;
    convertMovieToCube(movie, movieCube);

    // write something random to mmap file before calling mmap module
    {
        const char * msg = "hello";

        QFile file(QString::fromStdString(outputMemoryMapPath));
        bool success = file.open(QIODevice::ReadWrite);
        REQUIRE(success);

        auto numBytes = qstrlen(msg);
        auto bytesWritten = file.write(msg, numBytes);
        REQUIRE(bytesWritten == numBytes);

        file.close();
    }

    SECTION("Full FOV")
    {
        isx::writeMemoryMappedFileMovie(movie, outputMemoryMapPath);

        const std::tuple<size_t, size_t, size_t, size_t> roi = std::make_tuple(0, numRows - 1, 0, numCols - 1);
        isx::CubeFloat_t patch;
        isx::readMemoryMappedFileMovie(outputMemoryMapPath, numRows, numCols, numFrames, dataType, roi, patch);

        REQUIRE(arma::approx_equal(patch, movieCube, "reldiff", 1e-5f));
    }

    std::remove(outputMemoryMapPath.c_str());
}

TEST_CASE("MemoryMapMovieU16", "[cnmfe-utils]")
{
    const size_t numRows = 4;
    const size_t numCols = 3;
    const size_t numFrames = 5;
    const isx::DataType dataType = isx::DataType::U16;

    // Create a synthetic movie with regularly spaced element values
    const std::string inputMoviePath = "test/data/tmp_synthetic_movie_uint16.tif"; // movie dims: 3x4x5 (width * height * num_frames)
    {
        const arma::Col<uint16_t> data = arma::regspace<arma::Col<uint16_t>>(0, 1, numRows * numCols * numFrames);
        const arma::Cube<uint16_t> cube(data.memptr(), numRows, numCols, numFrames);

        saveCubeToTiffFile(cube, inputMoviePath);
    }

    const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(inputMoviePath));
    const std::string outputMemoryMapPath = "test/data/mmap.bin";
    
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
            std::make_tuple(0, 2, 0, 2),
            std::make_tuple(1, 3, 1, 2),
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
    std::remove(inputMoviePath.c_str());
}
