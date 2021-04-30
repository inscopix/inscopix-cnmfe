#include "catch.hpp"
#include "isxUtilities.h"


TEST_CASE("UtilitiesGetBaseName", "[utilities]")
{
    const std::string path = "/my/path/to/some/data/movie_128x128x100.tif";
    const std::string expectedBasename = "movie_128x128x100";
    const std::string actualBasename = isx::getBaseName(path);
    REQUIRE(expectedBasename == actualBasename);
}
