#ifndef ISX_MEMORY_MAPPING_UTILS_H
#define ISX_MEMORY_MAPPING_UTILS_H

#include "isxArmaUtils.h"
#include "isxTiffMovie.h"
#include "mio.hpp"

namespace isx
{
    /// Class representing a memory mapped movie
    ///
    class MemoryMappedMovie
    {
        public:
            /// Constructor
            ///
            MemoryMappedMovie(
                const mio::shared_mmap_source & inMmap,
                const size_t inOffset,
                const size_t inNumRows,
                const size_t inNumCols,
                const size_t inNumFrames)
                : m_mmap(inMmap)
                , m_offset(inOffset)
                , m_numRows(inNumRows)
                , m_numCols(inNumCols)
                , m_numFrames(inNumFrames)
            {}

            /// Returns a pointer to a 3D representation (numRows, numCols, numFrames) of the memory mapped movie
            /// The cube wraps around the external memory of the movie
            /// A pointer is returned to avoid copying the external memory
            /// auto ptr = movie->asCube();
            /// const CubeFloat_t & cube = (*ptr); 
            std::shared_ptr<CubeFloat_t> asCube();

            /// Returns a pointer to a 2D representation (numRows * numCols, numFrames) of the memory mapped movie
            /// The matrix wraps around the external memory of the movie
            /// A pointer is returned to avoid copying the external memory
            /// auto ptr = movie->asMat();
            /// const MatrixFloat & mat = (*ptr);
            std::shared_ptr<MatrixFloat_t> asMat();

        private:
            mio::shared_mmap_source m_mmap;         ///> Memory mapped file
            const size_t m_offset;                  ///> Offset in pixels indicating where to start reading
            const size_t m_numRows;                 ///> Number of rows in the field of view
            const size_t m_numCols;                 ///> Number of columns in the field of view
            const size_t m_numFrames;               ///> Number of frames
    };

    typedef std::shared_ptr<MemoryMappedMovie> SpMemoryMappedMovie_t;

    /// Writes a new memory mapped file of a movie
    ///
    /// \param inMovie                  Movie to memory map
    /// \param inFilenames              Filenames of memory mapped data
    /// \param inRois                   ROIs of movie to extract and sequentially write to memory map file
    /// \return                         True if the file was successfully and completely written to disk
    bool writeMemoryMapOfMovie(
        const SpTiffMovie_t & inMovie,
        const std::vector<std::string> & inFilenames,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & inRois);

    /// Reads an existing memory mapped file of a movie
    ///
    /// \param inFilenames              Filenames of memory mapped ROIs
    /// \param inOutRois                ROIs of movie to extract and sequentially write to memory map file
    /// \param numFrames                Number of frames in the movie
    /// \param outMemoryMappedMovies    Vector of memory mapped movies for each ROI of interest
    void readMemoryMapOfMovie(
        const std::vector<std::string> & inFilenames,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & inRois,
        const size_t numFrames,
        std::vector<SpMemoryMappedMovie_t> & outMemoryMappedMovies);

    /// Memory maps a movie
    /// Creates a new memory mapped file if the file does not exist
    /// Reads the memory mapped file into a vector of objects representing each input ROI
    ///
    /// \param inMovie                  Movie to memory map
    /// \param inFilenames              Filenames of memory mapped files (one per ROI)
    /// \param inOutRois                ROIs of movie to extract and write to individual memory map files
    /// \param outMemoryMappedMovies    Vector of memory mapped movies for each ROI of interest
    void memoryMapMovie(
        const SpTiffMovie_t & inMovies,
        const std::vector<std::string> & inFilenames,
        std::vector<std::tuple<size_t,size_t,size_t,size_t>> & inOutRois,
        std::vector<SpMemoryMappedMovie_t> & outMemoryMappedMovies);

} // namespace isx

#endif
