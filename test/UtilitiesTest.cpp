#include "catch.hpp"
#include "isxUtilities.h"
#include "isxTest.h"
#include "isxTiffMovie.h"
#include <iostream>
#include <fstream>

TEST_CASE("UtilitiesGetFileName", "[utilities]")
{
    const std::string path = "/my/path/to/some/data/movie_128x128x100.tif";
    const std::string expectedFileName = "movie_128x128x100.tif";
    const std::string actualFileName = isx::getFileName(path);
    REQUIRE(expectedFileName == actualFileName);
}

TEST_CASE("UtilitiesGetDirName", "[utilities]")
{
    SECTION("from full file path")
    {
        const std::string path = "/my/path/to/some/data/movie_128x128x100.tif";
        const std::string expectedDirName = "/my/path/to/some/data";
        const std::string actualDirName = isx::getDirName(path);
        REQUIRE(expectedDirName == actualDirName);
    }

    SECTION("from directory path ending with slash")
    {
        const std::string path = "/my/path/to/some/data/";
        const std::string expectedDirName = "/my/path/to/some/data";
        const std::string actualDirName = isx::getDirName(path);
        REQUIRE(expectedDirName == actualDirName);
    }

    SECTION("from directory path NOT ending with slash")
    {
        const std::string path = "/my/path/to/some/data";
        const std::string expectedDirName = "/my/path/to/some";
        const std::string actualDirName = isx::getDirName(path);
        REQUIRE(expectedDirName == actualDirName);
    }
}

TEST_CASE("UtilitiesPathExists", "[utilities]")
{
    SECTION("The path exists")
    {
        const std::string path = "test/data/movie.tif";
        REQUIRE(isx::pathExists(path));
    }

    SECTION("The path does not exist")
    {
        const std::string path = "random/data/file.bin";
        REQUIRE(!isx::pathExists(path));
    }
}

TEST_CASE("UtilitiesRemoveFiles", "[utilities]")
{
    const std::string path = "test/data/tmp.txt";
    REQUIRE(!isx::pathExists(path));

    std::ofstream outfile (path);
    outfile << "this is a temporary text file" << std::endl;
    outfile.close();
    REQUIRE(isx::pathExists(path));

    isx::removeFiles({path});
    REQUIRE(!isx::pathExists(path));
}

TEST_CASE("UtilitiesMakeRemoveDirectory", "[utilities]")
{
    const std::string path = "test/data/tmp";
    REQUIRE(!isx::pathExists(path));

    isx::makeDirectory(path);
    REQUIRE(isx::pathExists(path));

    isx::removeDirectory(path);
    REQUIRE(!isx::pathExists(path));
}

TEST_CASE("UtilitiesGetBaseName", "[utilities]")
{
    const std::string path = "/my/path/to/some/data/movie_128x128x100.tif";
    const std::string expectedBasename = "movie_128x128x100";
    const std::string actualBasename = isx::getBaseName(path);
    REQUIRE(expectedBasename == actualBasename);
}

TEST_CASE("TestingUtilitiesSaveCubeToTiffFileFloat32", "[testing-utilities]")
{
    const float data[60] = {
        1.03741539f, 21.85586756f, 16.67763465f, 57.27598156f, 38.64771696f,
        80.373977f, 64.95404632f, 87.34008431f, 61.05377408f, 18.73586471f,
        64.71167135f, 16.95314142f, 50.18745921f, 41.85081805f, 87.85590855f,
        89.80071182f, 69.44386778f, 98.33592482f, 86.08319232f, 90.62772769f,
        49.62817177f, 68.76740932f, 63.80539847f, 79.4547887f, 49.57732931f,
        24.81011684f, 95.09640316f, 66.68989729f, 82.48704645f, 16.96362576f,
        97.08460121f, 75.82779868f, 47.79740135f, 70.35291738f, 46.16976625f,
        16.26060466f, 13.3829529f,  8.40596512f,  3.87483758f, 54.7837783f,
        46.56684754f, 52.10847115f, 18.55416869f, 34.69019538f, 43.55554648f,
        92.52000773f,  5.30594245f, 13.87386998f, 14.21110855f, 34.54986401f,
        69.91073914f, 70.24274241f, 94.05244095f, 94.5517417f, 24.31156753f,
        46.3534714f, 87.04082964f,  3.94548177f,  3.85709232f, 35.29169532f
    };
    const isx::CubeFloat_t inputData(data, 4, 3, 5);
    const std::string outputFilename = "test/data/tmp_movie_float32.tif";

    SECTION("save float 32 movie")
    {
        saveCubeToTiffFile(inputData, outputFilename);

        // ensure file exists
        REQUIRE(isx::pathExists(outputFilename));

        // validate content of the file
        const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(outputFilename));
        for (size_t frameIndex = 0; frameIndex < movie->getNumFrames(); frameIndex++)
        {
            isx::MatrixFloat_t frame;
            movie->getFrame(frameIndex, frame);
            REQUIRE(arma::approx_equal(frame, inputData.slice(frameIndex), "reldiff", 1e-5f));
        }        
    }

    // delete file
    isx::removeFiles({outputFilename});
}

TEST_CASE("TestingUtilitiesSaveCubeToTiffFileUint16", "[testing-utilities]")
{
    const uint16_t data[60] = {
        1, 21, 16, 57, 38,
        80, 64, 87, 61, 18,
        64, 16, 50, 41, 87,
        89, 69, 98, 86, 90,
        49, 68, 63, 79, 49,
        24, 95, 66, 82, 16,
        97, 75, 47, 70, 46,
        16, 13,  8,  3, 54,
        46, 52, 18, 34, 43,
        92, 5, 13, 14, 34,
        69, 70, 94, 94, 24,
        46, 87,  3,  3, 35
    };
    const arma::Cube<uint16_t> inputData(data, 4, 3, 5);
    const std::string outputFilename = "test/data/tmp_movie_uint16.tif";;

    SECTION("save uint 16 movie")
    {
        saveCubeToTiffFile(inputData, outputFilename);

        // ensure file exists
        REQUIRE(isx::pathExists(outputFilename));

        // validate content of the file
        const isx::SpTiffMovie_t movie = std::shared_ptr<isx::TiffMovie>(new isx::TiffMovie(outputFilename));
        for (size_t frameIndex = 0; frameIndex < movie->getNumFrames(); frameIndex++)
        {
            arma::Mat<uint16_t> frame;
            movie->getFrame(frameIndex, frame);
            REQUIRE(arma::approx_equal(frame, inputData.slice(frameIndex), "abs_tol", 0));
        }
    }

    // delete file
    isx::removeFiles({outputFilename});
}

TEST_CASE("GetDirectoryName", "[cnmfe-io]")
{
    SECTION("relative path") {
        const std::string moviePath = "/my/path/to/some/directory/movie.tiff";
        const std::string expectedDirName = "/my/path/to/some/directory";
        std::string actualDirName = isx::getDirName(moviePath);
        REQUIRE(actualDirName == expectedDirName);
    }

    SECTION("current directory") {
        const std::string moviePath = ".";
        const std::string expectedDirName = "";
        std::string actualDirName = isx::getDirName(moviePath);
        REQUIRE(actualDirName == expectedDirName);
    }

    SECTION("empty string") {
        const std::string moviePath = "";
        const std::string expectedDirName = "";
        std::string actualDirName = isx::getDirName(moviePath);
        REQUIRE(actualDirName == expectedDirName);
    }

    SECTION("file in current directory") {
        const std::string moviePath = "movie.tiff";
        const std::string expectedDirName = "";
        std::string actualDirName = isx::getDirName(moviePath);
        REQUIRE(actualDirName == expectedDirName);
    }
}
