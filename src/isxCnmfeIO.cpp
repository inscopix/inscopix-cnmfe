#include "isxCnmfeIO.h"
#include <fstream>
#include <iomanip>

namespace isx
{
    void saveCnmfeOutputToH5File(
        const CubeFloat_t & footprints,
        const MatrixFloat_t & traces,
        const std::string outputFilename,
        const std::string footprintsKey,
        const std::string tracesKey)
    {
        // data is transposed since armadillo elements are stored in column-major ordering
        footprints.save(arma::hdf5_name(outputFilename, footprintsKey, arma::hdf5_opts::trans + arma::hdf5_opts::append));
        traces.save(arma::hdf5_name(outputFilename, tracesKey, arma::hdf5_opts::trans + arma::hdf5_opts::append));
    }

    void writeHeaders(
        std::ofstream & inStream,
        const std::string & cellNamePrefix,
        const size_t numCells)
    {
        inStream << "Frame";
        for (size_t cellId(0); cellId < numCells; cellId++)
        {
            inStream << "," << cellNamePrefix << std::to_string(cellId);
        }
        inStream << std::endl;
    }

    void writeTraces(
        std::ofstream & inStream,
        const MatrixFloat_t & inTraces)
    {
        const int32_t maxDecimalsForFloat = std::numeric_limits<float>::digits10 + 1;
        for (size_t timepoint(0); timepoint < inTraces.n_cols; timepoint++)
        {
            inStream << timepoint;
            for (size_t cellId(0); cellId < inTraces.n_rows; cellId++)
            {
                inStream << "," << std::setprecision(maxDecimalsForFloat) << inTraces(cellId, timepoint);
            }
            inStream << std::endl;
        }
    }

    void saveCnmfeTracesToCSVFile(
        const MatrixFloat_t & traces,
        const std::string outputFilename,
        const std::string cellNamePrefix)
    {
        if (traces.n_rows > 0)
        {
            std::ofstream outFile(outputFilename);
            writeHeaders(outFile, cellNamePrefix, traces.n_rows);
            writeTraces(outFile, traces);
            outFile.close();
        }
    }
}
