#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "isx/cnmfe.h"
#include "isxUtilities.h"

TEST_CASE("CnmfeFullTest", "[cnmfe]")
{
    const std::string inputMoviePath = "test/data/movie.tif";  // movie dims: 128x128x100 (width * height * num_frames)
    const std::string outputDirPath = "test/output";

    const std::string expFootprintsFile = outputDirPath + "/movie_footprints.tiff";
    const std::string expTracesFile = outputDirPath + "/movie_traces.csv";
    isx::removeFiles({expFootprintsFile, expTracesFile});

    const int outputFiletype = 0;  // tiff for footprints, csv for traces
    const int averageCellDiameter = 7;
    const float minCorr = 0.8;
    const float minPnr = 10.0;
    const int gaussianKernelSize = 0;  // auto estimate
    const int closingKernelSize = 0;   // auto estimate
    const int backgroundDownsamplingFactor = 2;
    const float ringSizeFactor = 1.4;
    const float mergeThreshold = 0.7;
    const int numThreads = 4;
    const int processingMode = 2;  // parallel patch mode
    const int patchSize = 80;
    const int patchOverlap = 20;
    const int traceOutputUnits = 1;  // noise scaled

    isx::cnmfe(
        inputMoviePath,
        outputDirPath,
        outputFiletype,
        averageCellDiameter,
        minCorr,
        minPnr,
        gaussianKernelSize,
        closingKernelSize,
        backgroundDownsamplingFactor,
        ringSizeFactor,
        mergeThreshold,
        numThreads,
        processingMode,
        patchSize,
        patchOverlap,
        traceOutputUnits);

    // a total of 79 cells should be identified
    // here we just check for the existence of the output files
    REQUIRE(isx::pathExists(expFootprintsFile));
    REQUIRE(isx::pathExists(expTracesFile));
    isx::removeFiles({expFootprintsFile, expTracesFile});
}
