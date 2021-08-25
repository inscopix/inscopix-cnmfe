#include "isx/cnmfe.h"
#include "json.hpp"
#include <fstream>

int main(int argc, char* argv[]) {
    const std::string inputMoviePath = std::string(argv[1]);
    const std::string inputCnmfeParams = std::string(argv[2]);
    const std::string outputDirPath = std::string(argv[3]);

    std::ifstream cnmfeParamsFile (inputCnmfeParams);
    nlohmann::json params;
    cnmfeParamsFile >> params;

    const int averageCellDiameter = params["average_cell_diameter"].get<int>();
    const float minCorr = params["min_pixel_correlation"].get<float>();
    const float minPnr = params["min_peak_to_noise_ratio"].get<float>();
    const int gaussianKernelSize = params["gaussian_kernel_size"].get<int>();
    const int closingKernelSize = params["closing_kernel_size"].get<int>();
    const int backgroundDownsamplingFactor = params["background_downsampling_factor"].get<int>();
    const float ringSizeFactor = params["ring_size_factor"].get<float>();
    const float mergeThreshold = params["merge_threshold"].get<float>();
    const int numThreads = params["number_of_threads"].get<int>();
    const int processingMode = params["processing_mode"].get<int>();
    const int patchSize = params["patch_size"].get<int>();
    const int patchOverlap = params["patch_overlap"].get<int>();
    const int traceOutputUnits = params["output_units"].get<int>();
    const int outputFiletype = params["output_filetype"].get<int>();

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
