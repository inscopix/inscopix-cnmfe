import os
import sys
import re
import subprocess
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

"""
This file was adapted from the pybind cmake examples:
https://github.com/pybind/cmake_example/blob/master/setup.py
"""

# convert distutils Windows platform specifiers to CMake -A arguments
WIN_PLATFORM_TO_CMAKE = {
      "win32": "Win32",
      "win-amd64": "x64",
      "win-arm32": "ARM",
      "win-arm64": "ARM64",
}

# compilers to use on Linux
LINUX_C_COMPILER = '/usr/bin/gcc-4.8'
LINUX_CXX_COMPILER = '/usr/bin/g++-4.8'

# A CMakeExtension needs a source directory instead of a file list.
# The name must be the single output extension from the CMake build.
class CMakeExtension(Extension):
      def __init__(self, name, sourcedir=""):
            Extension.__init__(self, name, sources=[])
            self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
      def build_extension(self, ext):
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

            # required for auto-detection & inclusion of auxiliary "native" libs
            if not extdir.endswith(os.path.sep):
                  extdir += os.path.sep

            debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
            cfg = "Debug" if debug else "Release"

            # CMake lets you override the generator - we need to check this.
            # Can be set with Conda-Build, for example.
            cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

            # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
            # from Python.
            cmake_args = [
                  "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={}".format(extdir),
                  "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                  "-DCMAKE_BUILD_TYPE={}".format(cfg),  # not used on MSVC, but no harm
            ]
            build_args = []
            # Adding CMake arguments set as environment variable
            # (needed e.g. to build for ARM OSx on conda-forge)
            if "CMAKE_ARGS" in os.environ:
                  cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ") if item]

            # specify that the python api must be built
            cmake_args += [
                  "-DBUILD_PYTHON_API=TRUE"
            ]

            if self.compiler.compiler_type != "msvc":
                  # Using Ninja-build since it a) is available as a wheel and b)
                  # multithreads automatically. MSVC would require all variables be
                  # exported for Ninja to pick it up, which is a little tricky to do.
                  # Users can override the generator with CMAKE_GENERATOR in CMake
                  # 3.15+.
                  if not cmake_generator:
                        try:
                              import ninja  # noqa: F401
                              cmake_args += ["-GNinja"]
                        except ImportError:
                              pass
            else:
                  # Single config generators are handled "normally"
                  single_config = any(x in cmake_generator for x in {"NMake", "Ninja"})

                  # CMake allows an arch-in-generator style for backward compatibility
                  contains_arch = any(x in cmake_generator for x in {"ARM", "Win64"})

                  # Specify the arch if using MSVC generator, but only if it doesn't
                  # contain a backward-compatibility arch spec already in the
                  # generator name.
                  if not single_config and not contains_arch:
                        cmake_args += ["-A", WIN_PLATFORM_TO_CMAKE[self.plat_name]]

                  # Multi-config generators have a different way to specify configs
                  if not single_config:
                        cmake_args += [
                              "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir)
                        ]
                        build_args += ["--config", cfg]

            if sys.platform.startswith("darwin"):
                  # Cross-compile support for macOS - respect ARCHFLAGS if set
                  archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
                  if archs:
                        cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]
            elif sys.platform.startswith("linux"):
                  cmake_args += [
                        "-DCMAKE_C_COMPILER={}".format(LINUX_C_COMPILER),
                        "-DCMAKE_CXX_COMPILER={}".format(LINUX_CXX_COMPILER)
                  ]

            # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
            # across all generators.
            if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
                  # self.parallel is a Python 3 only way to set parallel jobs by hand
                  # using -j in the build_ext call, not supported by pip or PyPA-build.
                  if hasattr(self, "parallel") and self.parallel:
                        # CMake 3.12+ only.
                        build_args += ["-j{}".format(self.parallel)]

            if not os.path.exists(self.build_temp):
                  os.makedirs(self.build_temp)

            subprocess.check_call(
                  ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp
            )
            subprocess.check_call(
                  ["cmake", "--build", "."] + build_args, cwd=self.build_temp
            )

setup(name='inscopix-cnmfe',
      version='1.0.0',
      author='Inscopix, Inc.',
      author_email="support@inscopix.com",
      url="https://github.com/inscopix/isx-cnmfe",
      description='Inscopix CNMFe for automated source extraction',
      classifiers=[
            'Operating System :: MacOS',
            'Operating System :: POSIX :: Linux',
            'Operating System :: Microsoft :: Windows',
            'Programming Language :: Python',
            'Programming Language :: Python :: 3',
            'License :: OSI Approved :: GNU Affero General Public License v3',
      ],
      platforms=["Mac OS-X", "Linux", "Windows"],
      python_requires='>=3.6',
      ext_modules=[CMakeExtension("inscopix_cnmfe_python")],
      cmdclass={"build_ext": CMakeBuild},
      install_requires=['numpy>=1.17.0'],
      zip_safe=False,
      license="GNU AFFERO GENERAL PUBLIC LICENSE",
      license_files="LICENSE")
