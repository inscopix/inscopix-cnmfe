# Building and Contributing to Inscopix CNMFe (for developers)
This section provides the necessary information for developers interested in contributing to the Inscopix CNMFe project.
This includes instructions for setting up the development environment, compiling the project into a static C++ library, 
packaging the project into a Python installable wheel file, and building a Docker image for the project.
A sample C++ program and unit tests are provided as well.

## Project structure
This section outlines the expected project structure and dependencies required to build Inscopix CNMFe.

### Libraries directory
The build instructions assume the dependencies are located in a subdirectory called *lib* as shown below.
This structure can be changed by updating the cmake files.
```
isx-cnmfe
 └── lib
      ├── armadillo
      ├── catch
      ├── hdf5
      ├── Intel_MKL
      ├── json
      ├── libtiff
      ├── mio
      ├── OpenCV
      ├── ThreadPool
      └── pybind11
```

### Dependencies
Below is a list of all the dependencies and corresponding versions used in Inscopix CNMFe.
Note that it may be compatible with other versions of the libraries, but only the ones listed here were tested.
pybind11 is only needed if you intend to build the Python API.

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
| [pybind11](https://github.com/pybind/pybind11) | 2.8.0 |

### Tested Systems
Below is a list of systems that were used to build and test Inscopix CNMFe along with its dependencies.
Note that it may be compatible with other systems, but only the ones listed below were tested.

| Operating System       | Compiler       |
| ------------- |:-------------:|
| macOS 10.15.7 | Apple Clang 8.0.0 (clang-800.0.38); XCode 8.0 |
| Ubuntu 18.04 LTS | GCC 4.8.5 (Ubuntu 4.8.5-4ubuntu8) |
| Windows 10 | Visual Studio 14 2015 Win64 (MSVC 19.0.24215.1) |

## Building Inscopix CNMFe as a static library
Below are platform-specific instructions for compiling CNMFe into a static C++ library.
The target platform can be specified using the CMake generator flag (-G).
The specific compilers to use can be specified by setting the appropriate CMake flags (CMAKE_C_COMPILER, CMAKE_CXX_COMPILER).

### Step 1: Create the build directory
From the root directory of the project, run the following commands to create a build directory and browse into it.
```
mkdir build
cd build
```

### Step 2: Build the project
Instructions for building the project on different operating systems are provided below.
These commands must be executed from within the build directory created in the previous step.

Mac
```
cmake ..
make
```

Linux
```
cmake -DCMAKE_C_COMPILER=/usr/bin/gcc-4.8 -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.8 ..
make
```

Windows
```
cmake -G "Visual Studio 14 2015 Win64" ..
# open build/cnmfe.sln in Visual Studio to build the project
```

## Example code for using the Inscopix CNMFe library
An example C++ project using the Inscopix CNMFe library is available in *example.cpp* and
compiled into an executable when building the app.
The following command will run CNMFe on a small movie recorded in the striatum.

Mac & Linux
```
./build/runCnmfe test/data/movie.tif test/data/params.json output
```

Windows
```
./build/Release/runCnmfe.exe test/data/movie.tif test/data/params.json output
```

### Running the unit tests
Once the project is built, unit tests can be run using the following command.

Mac & Linux
```
./build/test/runTests
```

Windows
```
./build/test/Release/runTests.exe
```

## Building and using the Inscopix CNMFe Docker image locally
To compile the app in a containerized environment, i.e. a Docker image:
```
docker build -t inscopix/cnmfe .
```

To run CNMFe within the container:
```
docker run --rm -ti \
	-v $PWD/test/data:/input \
	-v $PWD/output:/output \
    inscopix/cnmfe /input/movie.tif /input/params.json /output
```

## Building Inscopix CNMFe as an installable Python package
### Step 1: Create a Python virtual environment
```
conda create -n inscopix-cnmfe python=3.9
conda activate inscopix-cnmfe
```

Note that on Windows the environment must be created with the `anaconda` package:
```
conda create -n inscopix-cnmfe python=3.9 anaconda
```

### Step 2: Build the wheel file
By default the wheel file will be located in the distribution folder (`dist`).
```
python setup.py bdist_wheel
```

### Step 3: Install the inscopix_cnmfe package using the wheel file
The wheel filename may differ depending on the system and environment used to create it.
```
pip install dist/inscopix_cnmfe-1.0.0-cp39-cp39-macosx_10_12_x86_64.whl
```
You are now ready to use Inscopix CNMFe using Python.

### Usage in Python
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

## Troubleshooting
Below is a list of common issues you may run into while setting up your development environment
along with specific solutions to them.

| Operating System | Trigger | Error   | Solution       |
|:-------------|:------------------|:------------------| :-------------|
| Ubuntu | When building the C++ project | No rule to make target '/lib64/libz.so' | Create a symlink in /lib64 to libz.so <br> ```sudo ln -s /lib/x86_64-linux-gnu/libz.so.1 /lib64/libz.so``` |
| Windows | When running `import inscopix_cnmfe` in Python | ImportError: DLL load failed while importing inscopix_cnmfe: %1 is not a valid Win32 application. | Make sure `anaconda` is part of the Python environment in which you are installing inscopix-cnmfe. <br> `conda create -n inscopix-cnmfe python=3.9 anaconda`  |
