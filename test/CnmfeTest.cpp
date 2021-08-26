#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "isx/cnmfe.h"
#include "isxUtilities.h"
#include "isxArmaUtils.h"

#include <unistd.h>


TEST_CASE("CnmfeFullTest", "[cnmfe]")
{
    const std::string inputMoviePath = "test/data/movie_128x128x100.tif";
    const std::string outputDirPath = "test/output";
    const std::string outputPath = outputDirPath + "/movie_128x128x100_output.h5";

    const int outputFiletype = 1;  // .h5 file with keys "footprints" and "traces"
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
    isx::CubeFloat_t expA;
    isx::MatrixFloat_t expC;
    const std::string expectedOutputPath = "test/data/movie_128x128x100_output_df_noise.h5";
    expA.load(arma::hdf5_name(expectedOutputPath, "footprints", arma::hdf5_opts::trans));
    expC.load(arma::hdf5_name(expectedOutputPath, "traces", arma::hdf5_opts::trans));

    isx::CubeFloat_t actA;
    isx::MatrixFloat_t actC;
    actA.load(arma::hdf5_name(outputPath, "footprints", arma::hdf5_opts::trans));
    actC.load(arma::hdf5_name(outputPath, "traces", arma::hdf5_opts::trans));

    REQUIRE(arma::approx_equal(actA, expA, "reldiff", 1e-5f));
    REQUIRE(arma::approx_equal(actC, expC, "reldiff", 1e-5f));

    isx::removeDirectory(outputDirPath);
}
