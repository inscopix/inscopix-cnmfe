#include "isxCnmfeIO.h"

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
}
