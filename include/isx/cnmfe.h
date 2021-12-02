#ifndef ISX_CNMFE
#define ISX_CNMFE

#include <string>
#include <tuple>
#include <armadillo>

namespace isx
{
    ///  Applies the CNMFe algorithm to an input movie and saves the components identified to disk
    ///
    /// \param inputMoviePath               Path to the input movie (only tiff movie supported)
    /// \param outputDirPath                Path to output directory (intermediary files also stored there, e.g., memory map files)
    /// \param outputFileType               Filetype to use for saving the results of CNMFe (0: .tiff for footprints & .csv for traces, 1: .h5 file with keys "footprints" and "traces")
    /// \param averageCellDiameter          Average diameter of a neuron in pixels
    /// \param minCorr                      Minimum pixel correlation when searching for seed pixels
    /// \param minPnr                       Minimum peak-to-noise ratio when searching for seed pixels
    /// \param gaussianKernelSize           Width of Gaussian kernel used for spatial filtering (< 2 will be auto estimated)
    /// \param closingKernelSize            Morphological closing kernel size for spatial filtering of cell footprints (< 2 will be auto estimated)
    /// \param backgroundDownsamplingFactor Spatial downsampling factor used when estimating background
    /// \param ringSizeFactor               Ratio of ring radius to neuron diameter
    /// \param mergeThreshold               Temporal correlation threshold for merging spatially close cells
    /// \param numThreads                   Number of threads to use for processing
    /// \param processingMode               Processing mode for Cnmfe (0: all in memory, 1: sequential patches, 2: parallel patches)
    /// \param patchSize                    Size of a single patch in pixels
    /// \param patchOverlap                 Amount of overlap between patches in pixels
    /// \param traceOutputUnits             Output units for temporal components (0: DF, 1: noise scaled)
    /// \param verbose                      If true progress will be displayed in the console (0: false, 1: true)
    /// \param deconvolve                   If true deconvolved traces are returned (using OASIS AR(1)), otherwise raw traces are returned (0: raw traces, 1: deconvolved traces)
    std::tuple<arma::Cube<float>,arma::Mat<float>> cnmfe(
        const std::string & inputMoviePath,
        const std::string & outputDirPath = "output",
        const int outputFiletype = 0,
        const int averageCellDiameter = 7,
        const float minCorr = 0.8,
        const float minPnr = 10.0,
        const int gaussianKernelSize = 0,
        const int closingKernelSize = 0,
        const int backgroundDownsamplingFactor = 2,
        const float ringSizeFactor = 1.4,
        const float mergeThreshold = 0.8,
        const int numThreads = 4,
        const int processingMode = 2,
        const int patchSize = 80,
        const int patchOverlap = 20,
        const int traceOutputUnits = 1,
        const int deconvolve = 0,
        const int verbose = 0);
} // namespace isx

#endif // define ISX_CNMFE
