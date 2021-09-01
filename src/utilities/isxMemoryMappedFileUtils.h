#ifndef ISX_MEMORY_MAPPED_FILE_UTILS_H
#define ISX_MEMORY_MAPPED_FILE_UTILS_H

#include "isxTiffMovie.h"
#include "isxArmaUtils.h"

namespace isx
{
    /// Memory map writes a new binary file representing a movie
    /// The binary file stores raw data of each frame of the movie stored continguously in column-major form
    ///
    /// \param inMovies                 Movie with raw frame data to write
    /// \param inFilename               Filename of new binary file to memory map write
    void writeMemoryMappedFileMovie(
        const SpTiffMovie_t & inMovie,
        const std::string inFilename);

    /// Constructs a patch of a movie by memory map reading a binary file representing the movie
    /// The binary file should be created with writeMemoryMappedFileMovie(...)
    ///
    /// \param inFilename               Filename of binary file to memory map read
    /// \param inNumRows                Number of rows in a movie frame
    /// \param inNumCols                Number of columns in a movie frame
    /// \param inNumFrame               Number of frames in movie
    /// \param inDataType               Data type representing a pixel in movie
    /// \param inRoi                    ROI of movie to read
    /// \param outPatch                 Armadillo structure to store ROI of movie
    void readMemoryMappedFileMovie(
        const std::string inFilename,
        const size_t inNumRows,
        const size_t inNumCols,
        const size_t inNumFrames,
        const DataType inDataType,
        const std::tuple<size_t,size_t,size_t,size_t> & inRoi,
        CubeFloat_t & outPatch);
} // namespace isx

#endif
