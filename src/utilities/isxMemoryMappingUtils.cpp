#include "isxMemoryMappingUtils.h"
#include "isxUtilities.h"
#include "isxLog.h"

namespace isx
{
    std::shared_ptr<CubeFloat_t> MemoryMappedMovie::asCube()
    {
        return std::shared_ptr<CubeFloat_t>(new CubeFloat_t(
            (float*)m_mmap.data() + m_offset,
            m_numRows,
            m_numCols,
            m_numFrames,
            false,
            true
        ));
    }

    std::shared_ptr<MatrixFloat_t> MemoryMappedMovie::asMat()
    {
        return std::shared_ptr<MatrixFloat_t>(new MatrixFloat_t(
            (float*)m_mmap.data() + m_offset,
            m_numRows * m_numCols,
            m_numFrames,
            false,
            true
        ));
    }

    bool writeMemoryMapOfMovie(
        const SpTiffMovie_t & inMovie,
        const std::vector<std::string> & inFilenames,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & inRois)
    {
        if (inFilenames.size() != inRois.size())
        {
            ISX_LOG_ERROR("The number of memory-mapped filenames does not match the number of ROIs");
            throw std::invalid_argument("The number of memory-mapped filenames must match the number of ROIs.");
        }

        const size_t numPatches = inRois.size();
        std::vector<std::unique_ptr<std::fstream>> files;
        for (size_t i = 0; i < numPatches; i++)
        {
            files.emplace_back(new std::fstream(inFilenames[i], std::ios::out | std::ios::binary));
        }

        size_t movieFrame = 0;
        for (size_t frameId(0); frameId < inMovie->getNumFrames(); frameId++)
        {
            MatrixFloat_t frame;
            inMovie->getFrame(movieFrame, frame);
            movieFrame++;

            for (size_t p = 0; p < inRois.size(); p++)
            {
                std::tuple<size_t, size_t, size_t, size_t> roi = inRois[p];
                MatrixFloat_t fov = frame(arma::span(std::get<0>(roi), std::get<1>(roi)), arma::span(std::get<2>(roi), std::get<3>(roi)));
                files[p]->write((char*)fov.memptr(), fov.n_elem * sizeof(float));
            }
        }

        return true;
    }

    void readMemoryMapOfMovie(
        const std::vector<std::string> & inFilenames,
        const std::vector<std::tuple<size_t,size_t,size_t,size_t>> & inRois,
        const size_t numFrames,
        std::vector<SpMemoryMappedMovie_t> & outMemoryMappedMovies)
    {
        const size_t numPatches = inRois.size();
        size_t expectedSize = 0;
        size_t actualSize = 0;
        for (size_t i = 0; i < numPatches; i++)
        {
            std::error_code error;
            mio::shared_mmap_source mmap;
            mmap.map(inFilenames[i], error);
            if (error)
            {
                ISX_LOG_ERROR("Failed to memory map movie: ", error.message());
                throw std::runtime_error("Failed to memory map movie " + error.message());
            }

            const std::tuple<size_t, size_t, size_t, size_t> roi = inRois[i];
            size_t numRows = (std::get<1>(roi) - std::get<0>(roi) + 1);
            size_t numCols = (std::get<3>(roi) - std::get<2>(roi) + 1);
            outMemoryMappedMovies.emplace_back(new MemoryMappedMovie(
                mmap, 0, numRows, numCols, numFrames));
            expectedSize += numRows * numCols * numFrames;
            actualSize += mmap.size();
        }

        expectedSize *= sizeof(float);
        if (actualSize != expectedSize)
        {
            ISX_LOG_ERROR("The memory mapped file does not have the same dimensions as the ROIs");
            throw std::runtime_error("Memory mapped file does not have the same dimensions as the ROIs.");
        }
    }

    void memoryMapMovie(
        const SpTiffMovie_t & inMovie,
        const std::vector<std::string> & inFilenames,
        std::vector<std::tuple<size_t,size_t,size_t,size_t>> & inOutRois,
        std::vector<SpMemoryMappedMovie_t> & outMemoryMappedMovies)
    {
        if (inMovie == nullptr)
        {
            ISX_LOG_ERROR("Input movie not provided to memory mapping function");
            throw std::invalid_argument("Movie not provided.");
        }

        if (inFilenames.empty())
        {
            ISX_LOG_ERROR("Memory-mapped filenames not provided to memory mapping function");
            throw std::invalid_argument("Filenames for memory mapped files not provided");
        }

        const size_t numRows = inMovie->getFrameHeight();
        const size_t numCols = inMovie->getFrameWidth();
        const size_t numFrames = inMovie->getNumFrames();

        // check input rois
        for (const auto & roi : inOutRois)
        {
            if (std::get<0>(roi) >= numRows
                || std::get<1>(roi) >= numRows
                || std::get<2>(roi) >= numCols
                || std::get<3>(roi) >= numCols)
            {
                ISX_LOG_ERROR("ROI out of range of input movie dimensions");
                throw std::runtime_error("ROI out of range of input movie dimensions");
            }

            if (std::get<0>(roi) >= std::get<1>(roi)
                || std::get<2>(roi) >= std::get<3>(roi))
            {
                ISX_LOG_ERROR("Invalid ROI definition, ROI is not in the correct form");
                throw std::runtime_error("ROI is not in the correct form");
            }
        }

        if (inOutRois.empty())
        {
            // full fov
            inOutRois.emplace_back(0, numRows - 1, 0, numCols - 1);
        }

        bool filesExists = true; // check that every filename provided exists
        for (const auto & filename : inFilenames)
        {
            filesExists &= pathExists(filename);
        }

        // do not overwrite existing memory mapped files if they exists
        if (filesExists)
        {
            try
            {
                readMemoryMapOfMovie(inFilenames, inOutRois, numFrames, outMemoryMappedMovies);
            }
            catch (std::exception)
            {
                ISX_LOG_WARNING("Encountered incomplete or corrupt memory-mapped file, deleting and recreating the files");
                outMemoryMappedMovies.clear();
                removeFiles(inFilenames);
                writeMemoryMapOfMovie(inMovie, inFilenames, inOutRois);
                readMemoryMapOfMovie(inFilenames, inOutRois, numFrames, outMemoryMappedMovies);
            }
        }
        else // write new memory map files
        {
            removeFiles(inFilenames); // silently delete all files in case some currently exist
            writeMemoryMapOfMovie(inMovie, inFilenames, inOutRois);
            readMemoryMapOfMovie(inFilenames, inOutRois, numFrames, outMemoryMappedMovies);
        }
    }

} // namespace isx
