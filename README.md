# Inscopix CNMFe
Constrained non-negative matrix factorization algorithm for automated source extraction from microendoscopic data.

### Background
CNMFe is an algorithm used to perform automated source extraction from calcium imaging movies. Specifically, it aims to retrieve the spatial location and temporal dynamics of cells in a movie. The CNMF algorithm was originally developed for two-photon data [Pnevmatikakis, 2016](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4881387/). The two-photon algorithm was modified with enhanced background subtraction routines to work in the one-photon setting [Zhou, 2018](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5871355/). The first version of CNMFe, originally developed in MATLAB, was ported to Python in the CaImAn package, described in [Giovannucci, 2018](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6342523/). CaImAn CNMFe was ported to C++ for implementation into Inscopix Data Processing Software, improved for performance, and extended to offer greater processing control and transparency.

### Algorithm Overview
CNMFe is an iterative algorithm that solves a non-convex optimization problem, which consists of minimizing the difference between the input movie and the data reconstructed using the model. The process consists of making an initial estimation of the spatial location and temporal activity of cells, which we refer to as the initialization phase, and then refining this estimate over several iterations of processing.  The algorithm can be broken down into distinct and reusable processing modules as depicted below. 

The first two steps of Inscopix CNMFe aim to determine how to efficiently process the movie through division of labor and efficient memory management. The following 13 steps (steps 3 through 16) encompass the core functionality of CNMFe and are applied to the movie, or to spatially distinct portions of the movie for parallel processing. Finally, the results from parallel processing are combined and the components scaled based on user-specified parameters.

![CNMFe Modules Overview](img/cnmfe_modules_overview.png?raw=true "CNMFe Modules Overview")

### Building and Using Inscopix CNMFe
This package can be compiled into a static library for use in any application. A sample C++ project and unit tests are provided.

#### Expected directory structure
The build instructions assume the dependencies are located in a subdirectory called *lib* as shown below. This structure can be changed by updating the cmake files.
```
isx-cnmfe
    |
     -- lib
        |
         -- armadillo
         -- catch
         -- hdf5
         -- Intel_MKL
         -- json
         -- libtiff
         -- mio
         -- OpenCV
         -- ThreadPool
```

#### Building the static library
Instructions for compiling CNMFe into a static library.
```
mkdir build
cd build
cmake ..
make
```

#### Example code for using the CNMFe library
An example C++ project using the Inscopix CNMFe library is available in *example.cpp* and compiled into an executable when building the app. The following command will run CNMFe on a small movie recorded in the striatum. 
```
./build/runCnmfe data/movie.tif data/params.json output
```

#### Running the unit tests
Once the project is built, unit tests can be run using the following command.
```
./build/test/runTests
```

#### Using Inscopix CNMFe within a docker container
To compile the app in a containerized environment:
```
docker build -t inscopix/cnmfe .
```

To run CNMFe within the container:
```
docker run --rm -ti \
	-v $PWD/data:/input \
	-v $PWD/output:/output \
    inscopix/cnmfe /input/movie.tif /input/params.json /output
```

### Dependencies
Below is a list of all the dependencies used in Inscopix CNMFe. Note that it may be compatible with other versions of the libraries, but only the ones listed below were tested.

| Package       | Version       |
| ------------- |:-------------:|
| [armadillo](http://arma.sourceforge.net/) | 9.900.1 |
| [Intel MKL](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/onemkl.html#gs.1020r93) | 2017.2 |
| [OpenCV](https://opencv.org/) | 3.2.0 |
| [hdf5](https://support.hdfgroup.org/HDF5/doc/cpplus_RM/index.html) | 1.10 |
| [libtiff](https://libtiff.gitlab.io/libtiff/) | 4.0.8 |
| [ThreadPool](https://github.com/progschj/ThreadPool) | 1.0 |
| [mio](https://github.com/mandreyel/mio) | commit 8c0d3c7|
| [json](https://github.com/nlohmann/json) | 2.0.1 |
| [Catch](https://github.com/catchorg/Catch2) | 1.4.0 |
