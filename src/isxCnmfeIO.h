#ifndef ISX_CNMFE_IO_H
#define ISX_CNMFE_IO_H

#include "isxArmaUtils.h"

namespace isx
{
    ///
    /// \param footprints       Spatial footprints (d1 x d2 x K)
    /// \param traces           Temporal traces (K x T)
    /// \param outputFilename   Path to the output file (.h5)
    /// \param footprintsKey    Key used to store/retrieve the spatial footprints
    /// \param tracesKey        Key used to store/retrieve the temporal traces
    void saveCnmfeOutputToH5File(
        const CubeFloat_t & footprints,
        const MatrixFloat_t & traces,
        const std::string outputFilename,
        const std::string footprintsKey = "footprints",
        const std::string tracesKey = "traces");
} // namespace isx

#endif //ISX_CNMFE_IO_H
