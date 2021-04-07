#ifndef ISX_CNMFE_IO_H
#define ISX_CNMFE_IO_H

#include "isxArmaUtils.h"

namespace isx
{
    /// Saves the output of cnmfe to a hierarchical data format file
    ///
    /// \param footprints       Spatial footprints (d1 x d2 x K)
    /// \param traces           Temporal traces (K x T)
    /// \param outputFilename   Path to the output file (.h5)
    /// \param footprintsKey    Key used to store/retrieve the spatial footprints
    /// \param tracesKey        Key used to store/retrieve the temporal traces
    void saveOutputToH5File(
        const CubeFloat_t & footprints,
        const MatrixFloat_t & traces,
        const std::string & outputFilename,
        const std::string & footprintsKey = "footprints",
        const std::string & tracesKey = "traces");

    /// Saves cnmfe temporal traces to a csv file
    /// (one row per timepoint, one column per cell)
    ///
    /// \param traces           Temporal traces (K x T)
    /// \param outputFilename   Path to the output file (.csv)
    /// \param cellNamePrefix   Prefix to use for each cell name (suffix is the cell id)
    void saveTracesToCSVFile(
        const MatrixFloat_t & traces,
        const std::string & outputFilename,
        const std::string & cellNamePrefix = "C");

    ///  Saves cnmfe footprints to a tiff file
    ///
    /// \param footprints       Spatial footprints (d1 x d2 x K)
    /// \param outputFilename   Path to the output file (.tiff)
    void saveFootprintsToTiffFile(
        const CubeFloat_t & footprints,
        const std::string & outputFilename);
} // namespace isx

#endif //ISX_CNMFE_IO_H
