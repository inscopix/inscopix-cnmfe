#ifndef ISX_MEMORY_MAPPED_FILE_UTILS_H
#define ISX_MEMORY_MAPPED_FILE_UTILS_H

#include "isxTiffMovie.h"
#include "isxArmaUtils.h"

namespace isx
{
    /// Memory maps a new binary file
    /// Writes raw frame data of a movie to the binary file
    /// Each frame of the movie is stored contingously in memory and in column-major form
    ///
    /// \param inMovies                 Movie with raw frame data to write
    /// \param inFilename               Filename of new binary file
    void writeMemoryMappedFileMovie(
        const SpTiffMovie_t & inMovie,
        const std::string inFilename);

    /// Memory maps a binary file representing a movie
    /// Contructs a patch of the movie by reading binary file
    /// The binary file should be created with writeMemoryMappedFileMovie(...)
    ///
    /// \param inFilename               Filename of binary file
    /// \param inNumRows                Number of rows in a movie frame
    /// \param inNumCols                Number of columns in a movie frame
    /// \param inNumFrame               Number of frames in movie
    /// \param inDataType               Data type representing a pixel in movie
    /// \param inRoi                    Rectangular region of interest defined as (start row index, end row index, start col index, end col index) to read from the movie
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
