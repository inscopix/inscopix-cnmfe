# Inscopix-CNMFe
Inscopix-CNMFe is a constrained non-negative matrix factorization (CNMF) algorithm used to perform automated source extraction from microendoscopic calcium imaging movies (the 'e' stands for endoscope). Specifically, it aims to retrieve the spatial location and temporal dynamics of neurons in a fluorescent 1-photon calcium imaging movie. 

![CNMFe Workflow Example](img/inscopix_cnmfe_workflow.png?raw=true "CNMFe Workflow Example")

Inscopix-CNMFe is implemented in C++, and can be used out of the box in Python via `pip install` or with a Docker container. The source code is open and available to developers who wish to further evaluate, modify, and improve the algorithm.

## Background
The CNMF algorithm was originally developed for two-photon data by [Pnevmatikakis, 2016](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4881387/). 
The two-photon algorithm was modified with enhanced background subtraction routines to work in the one-photon setting by [Zhou, 2018](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5871355/). 
The first version of CNMFe, originally developed in MATLAB as [CNMF_E](https://github.com/zhoupc/CNMF_E), was ported to Python in the [CaImAn](https://github.com/flatironinstitute/CaImAn) package, described in [Giovannucci, 2018](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6342523/). 
The Inscopix Data Products and Analytics team then reimplemented the constrained non-negative matrix factorization approach in C++ (Inscopix-CNMFe) to offer in the Inscopix Data Processing Software GUI as well as its Python and MATLAB APIs. Here we've packaged Inscopix-CNMFe into a standalone open source software for anyone to use.

## Installation & Usage

### Using Python
Inscopix-CNMFe can be installed directly into your Python environment using the commands below.
These commands assume you have pre-installed [Anaconda](https://www.anaconda.com/products/individual), which can be used to create and manage Python environments.
Note that Inscopix-CNMFe is currently available only for Python version 3.6 or higher on 64-bit machines,
with the exception of Python 3.10 on Windows which isn't compatible yet.

```
conda create -n inscopix-cnmfe python=3.9
conda activate inscopix-cnmfe
pip install inscopix-cnmfe
```

Note that on Windows the environment must include the `anaconda` package, which can be installed while creating the environment:
```
conda create -n inscopix-cnmfe python=3.9 anaconda
```

Once Inscopix-CNMFe is installed in your Python environment, you can run CNMFe on a given movie as follows:
```
import inscopix_cnmfe

footprints, traces = inscopix_cnmfe.run_cnmfe(
    input_movie_path='test/data/movie.tif', 
    output_dir_path='output', 
    output_filetype=0,
    average_cell_diameter=7,
    min_corr=0.8,
    min_pnr=10.0,
    gaussian_kernel_size=0,
    closing_kernel_size=0,
    background_downsampling_factor=2,
    ring_size_factor=1.4,
    merge_threshold=0.7,
    num_threads=4,
    processing_mode=2,
    patch_size=80,
    patch_overlap=20,
    trace_output_units=1,
    deconvolve=0,
    verbose=1
)
```

#### Example Notebook
A demo Jupyter Notebook that runs Inscopix-CNMFe on a small movie and displays spatial footprints 
and temporal traces side by side is available [here](Inscopix_CNMFe_Demo.ipynb).

### Using Docker

**Step 1: Install [Docker](https://docs.docker.com/get-docker/)**

**Step 2: Run Inscopix CNMFe within a docker container**

Place the input movie and the CNMFe parameters stored in a json file in the same folder.
Note that only tiff movies are currently supported.
The docker image will automatically be downloaded the first time you run the command.
```
docker run --rm -ti \
	-v $PWD/test/data:/input \
	-v $PWD/output:/output \
	public.ecr.aws/inscopix/cnmfe /input/movie.tif /input/params.json /output
```

## Algorithm Overview
CNMFe is an iterative algorithm that solves a non-convex optimization problem, which consists of minimizing the difference between the input movie and the data reconstructed using the model. The process consists of making an initial estimation of the spatial location and temporal activity of cells, which we refer to as the initialization phase, and then refining this estimate over several iterations of processing. The algorithm can be broken down into distinct and reusable processing modules as depicted below. 

The first two steps of Inscopix-CNMFe aim to determine how to efficiently process the movie through division of labor and efficient memory management. The following 13 steps (steps 3 through 16) encompass the core functionality of CNMFe and are applied to the movie, or to spatially distinct portions of the movie for parallel processing. Finally, the results from parallel processing are combined and the components scaled based on user-specified parameters.

![CNMFe Modules Overview](img/cnmfe_modules_overview.png?raw=true "CNMFe Modules Overview")

## Recommended Workflow
Prior to running Inscopix-CNMFe, we recommend applying the following operations
to the input movie to help improve the performance of the source extraction algorithm.
1. Temporal downsampling of the data to 10 Hz or below. On each iteration of the algorithm,
   temporal traces are deconvolved using an autoregressive model of order 1. Higher-frequency data
   may not be adequately deconvolved using low-order models in noisy regimes. The current version of Inscopix-CNMFe
   performs deconvolution using the [OASIS](https://pubmed.ncbi.nlm.nih.gov/28291787/) algorithm with an AR(1) model,
   which is appropriate for data recorded at up to 10 Hz.
2. Spatial downsampling of the data by a factor of 2 to 4. This will help blur away
   minor spatial fluctuations and significantly reduce CNMFe processing time.
3. Spatial bandpass filtering with global mean subtraction. The removal of low spatial
   frequency content will help remove out-of-focus cells. The removal of
   high spatial frequencies will reduce noise by smoothing the movie images.
4. Motion correction. The removal of motion artifacts will help ensure that the spatial
   location of cells identified by CNMFe is confined to their precise positions as
   opposed to the pixels visited by their respective cell body over time.
   This will in turn ensure that the temporal dynamics extracted for each cell are due to
   fluctuations in the fluorescent reporter and not cellular displacements.

## Algorithm Parameters
The CNMFe parameters used in the Python package are listed below along with their respective descriptions and default values.
All parameters are optional with the exception of the input movie file and the average cell diameter.
Note that the default values may not be optimal for all scenarios and should be adjusted based on the results obtained by the algorithm.

| Parameter  | Description | Default Value |
|:----------|:-------------|:-------------|
| input_movie_path | path to the input tiff movie file | N/A |
| average_cell_diameter | the average cell diameter of a representative cell in pixels | 7 |
| min_pixel_correlation | the minimum correlation of a pixel with its immediate neighbors when searching for new cell centers | 0.8 |
| min_peak_to_noise_ratio | the minimum peak-to-noise ratio of a pixel when searching for new cell centers | 10 |
| gaussian_kernel_size | the width in pixels of the Gaussian kernel used for spatial filtering of the movie before cell initialization <br/> (automatically estimated when the value provided is smaller than 3) | 0 |
| closing_kernel_size | the size in pixels of the morphological closing kernel used for removing small disconnected components and connecting small cracks within individual cell footprints <br/> (automatically estimated when the value provided is smaller than 3) | 0 |
| background_downsampling_factor | the spatial downsampling factor to use when estimating the background activity | 2 |
| ring_size_factor | the multiple of the average cell diameter to use for computing the radius of the ring model used for estimating the background activity | 1.4 |
| merge_threshold | the temporal correlation threshold for merging cells that are spatially close | 0.7 |
| number_of_threads | the number of threads to use for processing | 4 |
| processing_mode | the processing mode to use to run CNMFe (0: all in memory, 1: sequential patches, 2: parallel patches) <br/><br/><ul><li>All in memory: processes the entire field of view at once.</li><li>Sequential patches: breaks the field of view into overlapping patches and processes them one at a time using the specified number of threads where parallelization is possible.</li><li>Parallel patches:  breaks the field of view into overlapping patches and processes them in parallel using a single thread for each.</li></ul>| 2 |
| patch_size | the side length of an individual square patch of the field of view in pixels | 80 |
| patch_overlap | the amount of overlap between adjacent patches in pixels | 20 |
| deconvolve | specifies whether to deconvolve the final temporal traces (0: return raw traces, 1: return deconvolved traces) | 0 |
| output_units | the units of the output temporal traces (0: dF, 1: dF over noise) <br/><br/><ul><li>dF: temporal traces on the same scale of pixel intensity as the original movie. dF is calculated as the average fluorescence activity of all pixels in a cell, scaled so that each spatial footprint has a magnitude of 1.</li><li>dF over noise: temporal traces divided by their respective estimated noise level. This can be interpreted similarly to a z-score, with the added benefit that the noise is a more robust measure of the variance in a temporal trace compared to the standard deviation.</li></ul> | 1 |
| output_filetype | the file types into which the output will be saved (0: footprints saved to a tiff file and traces saved to a csv file, 1: output saved to a h5 file under the keys footprints and traces) | 0 |
| output_dir_path | path to the directory where output files will be stored (empty string: output files not saved to disk) | empty string |

## Tuning Parameters to Optimize Performance
To learn more about the effect of each parameter on the algorithm or to determine the best course of action
for fine-tuning parameters based on the results obtained by the algorithm, please consult our documentation
on Inscopix-CNMFe Parameters [here](docs/parameter_tuning.md).

## Contribute to Inscopix CNMFe
For those interested in contributing to this project, please consult our documentation for developers [here](docs/developers_guide.md).

## Project Team
This project was led by Bruno Boivin, Nosheen Adil, and Shay Neufeld in the Inscopix Data Products & Analytics Group, with many others across the company contributing along the way.

Developers: 
- Bruno Boivin, Data Engineer
- Nosheen Adil, Software Engineer
- Eric Lin, Software Engineering intern

## License
This program is free software; you can use, redistribute, and/or modify it under the terms of the GNU Affero General Public License. Note that the software is provided 'as is', without warranty of any kind. See the Affero General Public License for more details. 
