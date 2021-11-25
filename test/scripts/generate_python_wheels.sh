#!/bin/bash

# This script generates and tests the installable wheel files for all Python versions supported on Mac OS by Inscopix CNMF-E.
# It is designed to be executed on Mac OS or Linux from the root directory of the project.
# Usage: ./test/scripts/generate_python_wheels.sh

PYTHON_VERSIONS=(3.6 3.7 3.8 3.9 3.10)
for PYTHON_VERSION in "${PYTHON_VERSIONS[@]}"
do
CONDA_ENV_NAME=inscopix-cnmfe-${PYTHON_VERSION}
conda create -n ${CONDA_ENV_NAME} python=${PYTHON_VERSION} -y
conda activate ${CONDA_ENV_NAME}
conda run -n ${CONDA_ENV_NAME} python setup.py bdist_wheel
conda run -n ${CONDA_ENV_NAME} pip install dist/*cp${PYTHON_VERSION//./}*
conda run -n ${CONDA_ENV_NAME} python -c "import inscopix_cnmfe; inscopix_cnmfe.run_cnmfe('test/data/movie.tif')"
conda env remove -n ${CONDA_ENV_NAME}
done
