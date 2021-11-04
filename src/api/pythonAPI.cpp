#include <pybind11/pybind11.h>
#include "isx/cnmfe.h"

namespace py = pybind11;

PYBIND11_MODULE(inscopix_cnmfe, handle)
{
    handle.doc() = "Inscopix CNMFe for automated source extraction";
    handle.def("run_cnmfe", &isx::cnmfe, R"mydelimiter(
    Run the CNMFe cell identification algorithm on a movie

    Arguments
    ---------
    input_movie_path (str): Path to the input tiff movie file
    output_dir_path (str): Path to output directory
    output_filetype (int): Filetype to use for saving the results of CNMFe (0: .tiff for footprints & .csv for traces, 1: .h5 file with keys "footprints" and "traces")
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
    verbose (int): To enable and disable verbose mode. When enabled, progress is displayed in the console. (0: disabled, 1: enabled)
    )mydelimiter",
    py::arg("input_movie_path"),
    py::arg("output_dir_path") = "output",
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
    py::arg("verbose") = 0
    );
}
