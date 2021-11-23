#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "isx/cnmfe.h"

namespace py = pybind11;

/// The helper functions for converting between Armadillo data structures
/// and pybind arrays were adapted from the carma library: https://github.com/RUrlus/carma

// py::capsule holds a reference to a Python object (with reference counting)
template <typename armaT>
inline py::capsule create_capsule(armaT* data)
{
    return py::capsule(data, [](void* f) {
        auto mat = reinterpret_cast<armaT*>(f);
        delete mat;
    });
}

// Dummy reference to a Python object
template <typename armaT>
inline py::capsule create_dummy_capsule(const armaT* data)
{
    return py::capsule(data, [](void*) {});
}

// Construct a py::array from an Armadillo Cube
template <typename T>
inline py::array_t<T> construct_array(const arma::Cube<T>* data)
{
    constexpr auto tsize = static_cast<ssize_t>(sizeof(T));
    auto nrows = static_cast<ssize_t>(data->n_rows);
    auto ncols = static_cast<ssize_t>(data->n_cols);
    auto nslices = static_cast<ssize_t>(data->n_slices);
    auto arr = py::array_t<T>(
        {nslices, nrows, ncols},                       // shape
        {tsize * nrows * ncols, tsize, nrows * tsize}, // F-style contiguous strides
        data->memptr(),                                // data pointer
        create_dummy_capsule<arma::Cube<T>>(data)      // numpy array references this parent
    );
    return arr;
}

// Construct a py::array from an Armadillo Matrix
template <typename T>
inline py::array_t<T> construct_array(arma::Mat<T>* data)
{
    constexpr auto tsize = static_cast<ssize_t>(sizeof(T));
    auto nrows = static_cast<ssize_t>(data->n_rows);
    auto ncols = static_cast<ssize_t>(data->n_cols);

    py::capsule base = create_capsule<arma::Mat<T>>(data);

    return py::array_t<T>(
        {nrows, ncols},          // shape
        {tsize, nrows * tsize},  // F-style contiguous strides
        data->memptr(),          // the data pointer
        base                     // numpy array references this parent
    );
}

// Convert an Armadillo Matrix to a pybind py::array
template <typename T>
inline py::array_t<T> armaMatToPyarray(const arma::Mat<T>& src)
{
    return construct_array<T>(new arma::Mat<T>(src));
}

// Convert an Armadillo Cube to a pybind py::array
template <typename T>
inline py::array_t<T> armaCubeToPyarray(arma::Cube<T>& src)
{
    arma::Cube<T>* data = new arma::Cube<T>(std::move(src));
    return construct_array<T>(data);
}

std::tuple<py::array,py::array> isx_cnmfe_python(
    const std::string & inputMoviePath,
    const std::string & outputDirPath,
    const int outputFiletype,
    const int averageCellDiameter,
    const float minCorr,
    const float minPnr,
    const int gaussianKernelSize,
    const int closingKernelSize,
    const int backgroundDownsamplingFactor,
    const float ringSizeFactor,
    const float mergeThreshold,
    const int numThreads,
    const int processingMode,
    const int patchSize,
    const int patchOverlap,
    const int traceOutputUnits,
    const int deconvolve,
    const int verbose)
    {
    std::tuple<arma::Cube<float>,arma::Mat<float>> cnmfeOutput = isx::cnmfe(
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
        traceOutputUnits,
        deconvolve,
        verbose
    );

    py::array footprints = armaCubeToPyarray(std::get<0>(cnmfeOutput));
    py::array traces = armaMatToPyarray(std::get<1>(cnmfeOutput));
    return std::make_tuple(footprints, traces);
}

PYBIND11_MODULE(inscopix_cnmfe, handle)
{
    handle.doc() = "Inscopix CNMF-E for automated source extraction";
    handle.def("run_cnmfe", &isx_cnmfe_python, R"mydelimiter(
    Run the CNMF-E cell identification algorithm on a movie

    Arguments
    ---------
    input_movie_path (str): Path to the input tiff movie file
    output_dir_path (str): Path to output directory (output files not saved when given an empty string)
    output_filetype (int): Filetype to use for saving the results of CNMF-E (0: .tiff for footprints & .csv for traces, 1: .h5 file with keys "footprints" and "traces")
    average_cell_diameter (int): Average diameter of a neuron in pixels
    min_corr (float): Minimum pixel correlation when searching for seed pixels
    min_pnr (float): Minimum peak-to-noise ratio when searching for seed pixels
    gaussian_kernel_size (int): Width of Gaussian kernel used for spatial filtering (< 2 will be auto estimated)
    closing_kernel_size (int): Morphological closing kernel size for spatial filtering of cell footprints (< 2 will be auto estimated)
    background_downsampling_factor (int): Spatial downsampling factor used when estimating background
    ring_size_factor (float): Ratio of ring radius to neuron diameter
    merge_threshold (float): Temporal correlation threshold for merging spatially close cells
    num_threads (int): Number of threads to use for processing
    processing_mode (int): Processing mode for Cnmfe (0: all in memory, 1: sequential patches, 2: parallel patches)
    patch_size (int): Size of a single patch in pixels
    patch_overlap (int): Amount of overlap between patches in pixels
    trace_output_units (int): Output units for temporal components (0: DF, 1: noise scaled)
    deconvolve (int): Specifies whether to deconvolve the final temporal traces (0: return raw traces, 1: return deconvolved traces)
    verbose (int): To enable and disable verbose mode. When enabled, progress is displayed in the console. (0: disabled, 1: enabled)
    )mydelimiter",
    py::arg("input_movie_path"),
    py::arg("output_dir_path") = "",
    py::arg("output_filetype") = 0,
    py::arg("average_cell_diameter") = 7,
    py::arg("min_corr") = 0.8,
    py::arg("min_pnr") = 10.0,
    py::arg("gaussian_kernel_size") = 0,
    py::arg("closing_kernel_size") = 0,
    py::arg("background_downsampling_factor") = 2,
    py::arg("ring_size_factor") = 1.4,
    py::arg("merge_threshold") = 0.7,
    py::arg("num_threads") = 4,
    py::arg("processing_mode") = 2,
    py::arg("patch_size") = 80,
    py::arg("patch_overlap") = 20,
    py::arg("trace_output_units") = 1,
    py::arg("deconvolve") = 0,
    py::arg("verbose") = 0
    );
}
