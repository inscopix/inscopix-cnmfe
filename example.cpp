#include "isx/cnmfe.h"

int main() {

    const std::string inputMoviePath = "test/data/movie_128x128x100.tif";
    const std::string outputDirPath = "isx-cnmfe_output";
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

	return 0;
}
