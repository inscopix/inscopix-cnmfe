# Inscopix CNMFe
CNMFe is a constrained non-negative matrix factorization algorithm used to perform automated source extraction from microendoscopic calcium imaging movies. 
Specifically, it aims to retrieve the spatial location and temporal dynamics of neurons in a movie. 
The CNMF algorithm was originally developed for two-photon data [Pnevmatikakis, 2016](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4881387/). 
The two-photon algorithm was modified with enhanced background subtraction routines to work in the one-photon setting [Zhou, 2018](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5871355/). 
The first version of CNMFe, originally developed in MATLAB as [CNMF_E](https://github.com/zhoupc/CNMF_E), was ported to Python in the [CaImAn](https://github.com/flatironinstitute/CaImAn) package, described in [Giovannucci, 2018](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6342523/). 
CaImAn CNMFe was ported to C++ for implementation into Inscopix Data Processing Software, improved for performance, and extended to offer greater processing control and transparency.

![CNMFe Workflow Example](img/inscopix_cnmfe_workflow.png?raw=true "CNMFe Workflow Example")

## Installation & Usage

### Using Python
Inscopix CNMFe can be installed directly into your Python environment using the commands below.
These commands assume you have pre-installed [Anaconda](https://www.anaconda.com/products/individual), which can be used to create and manage Python environments.
Note that Inscopix CNMFe is currently available only for Python version 3.6 or higher on 64-bit machines,
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

Once Inscopix CNMFe is installed in your Python environment, you can run CNMFe on a given movie as follows:
```
import inscopix_cnmfe

inscopix_cnmfe.run_cnmfe(
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
	trace_output_units=1
)
```

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

The first two steps of Inscopix CNMFe aim to determine how to efficiently process the movie through division of labor and efficient memory management. The following 13 steps (steps 3 through 16) encompass the core functionality of CNMFe and are applied to the movie, or to spatially distinct portions of the movie for parallel processing. Finally, the results from parallel processing are combined and the components scaled based on user-specified parameters.

![CNMFe Modules Overview](img/cnmfe_modules_overview.png?raw=true "CNMFe Modules Overview")

## Description of Individual Parameters
The CNMFe parameters used in this package are listed below along with their respective descriptions.

| Parameter  | Description |
| ----------:|:-------------|
| average_cell_diameter | the average cell diameter of a representative cell in pixels |
| min_pixel_correlation | the minimum correlation of a pixel with its immediate neighbors when searching for new cell centers |
| min_peak_to_noise_ratio | the minimum peak-to-noise ratio of a pixel when searching for new cell centers |
| gaussian_kernel_size | the width in pixels of the Gaussian kernel used for spatial filtering of the movie before cell initialization |
| closing_kernel_size | the size in pixels of the morphological closing kernel used for removing small disconnected components and connecting small cracks within individual cell footprints |
| background_downsampling_factor | the spatial downsampling factor to use when estimating the background activity |
| ring_size_factor | the multiple of the average cell diameter to use for computing the radius of the ring model used for estimating the background activity |
| merge_threshold | the temporal correlation threshold for merging cells that are spatially close |
| number_of_threads | the number of threads to use for processing |
| processing_mode | the processing mode to use to run CNMFe (0: all in memory, 1: sequential patches, 2: parallel patches) <br/><br/><ul><li>All in memory: processes the entire field of view at once.</li><li>Sequential patches: breaks the field of view into overlapping patches and processes them one at a time using the specified number of threads where parallelization is possible.</li><li>Parallel patches:  breaks the field of view into overlapping patches and processes them in parallel using a single thread for each.</li></ul>|
| patch_size | the side length of an individual square patch of the field of view in pixels |
| patch_overlap | the amount of overlap between adjacent patches in pixels |
| output_units | the units of the output temporal traces (0: dF, 1: dF over noise) <br/><br/><ul><li>dF: temporal traces on the same scale of pixel intensity as the original movie. dF is calculated as the average fluorescence activity of all pixels in a cell, scaled so that each spatial footprint has a magnitude of 1.</li><li>dF over noise: temporal traces divided by their respective estimated noise level. This can be interpreted similarly to a z-score, with the added benefit that the noise is a more robust measure of the variance in a temporal trace compared to the standard deviation.</li></ul> |
| output_filetype | the file types into which the output will be saved (0: footprints saved to a tiff file and traces saved to a csv file, 1: output saved to a h5 file under the keys footprints and traces) |

## Contribute to Inscopix CNMFe
For those interested in contributing to this project, please consult our documentation for developers [here](docs/developers.md).