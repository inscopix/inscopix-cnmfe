#include "catch.hpp"
#include "isxUtilities.h"

#include <QFile>

TEST_CASE("UtilitiesGetFileName", "[utilities]")
{
    const std::string path = "/my/path/to/some/data/movie_128x128x100.tif";
    const std::string expectedFileName = "movie_128x128x100.tif";
    const std::string actualFileName = isx::getFileName(path);
    REQUIRE(expectedFileName == actualFileName);
}

TEST_CASE("UtilitiesGetDirName", "[utilities]")
{
    const std::string path = "/my/path/to/some/data/movie_128x128x100.tif";
    const std::string expectedDirName = "/my/path/to/some/data";
    const std::string actualDirName = isx::getDirName(path);
    REQUIRE(expectedDirName == actualDirName);
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

    {
        const char * msg = "hello";

        QFile file(QString::fromStdString(path));
        bool success = file.open(QIODevice::ReadWrite);
        REQUIRE(success);

        auto numBytes = qstrlen(msg);
        auto bytesWritten = file.write(msg, numBytes);
        REQUIRE(bytesWritten == numBytes);

        file.close();
    }
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
