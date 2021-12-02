#include "isxMemoryMappedFileUtils.h"

#include "mio.hpp"

namespace isx
{
    void writeMemoryMappedFileMovie(
        const SpTiffMovie_t & inMovie,
        const std::string inFilename)
    {
        const size_t numRows = inMovie->getFrameHeight();
        const size_t numCols = inMovie->getFrameWidth();
        const size_t numFrames = inMovie->getNumFrames();

        const DataType dataType = inMovie->getDataType();
        size_t numBytes = (numRows * numCols * numFrames);
        if (dataType == DataType::U16)
        {
            numBytes *= sizeof(uint16_t);
        }
        else if (dataType == DataType::F32)
        {
            numBytes *= sizeof(float);
        }
        else
        {
            const std::string errorMessage = "writeMemoryMappedFileMovie: No conversion specified from data type (" + std::to_string(int(dataType)) + ") to float.";
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        // In case the tmp file was not removed successfully on a previous run of CNMFe
        // Remove the file if it exists
        if (pathExists(inFilename))
        {
            std::remove(inFilename.c_str());
        }

        // This file contains frames organized sequentially on disk that are used for memory mapping.
        // Use std library instead of mio to write this file because mio expects the file to exist
        // and for the size of the file to be preallocated with the number of bytes to write to the file (i.e., the total number of bytes of the movie).
        std::ofstream file;
        file.open(inFilename, std::ofstream::binary | std::ofstream::out);
        if (!file.good() || !file.is_open())
        {
            const std::string errorMessage = "Failed to open memory mapped file for writing: " + inFilename + "\nError from standard library: " + std::generic_category().message(errno);
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        for (size_t frameIndex = 0; frameIndex < numFrames; frameIndex++)
        {
            if (dataType == DataType::U16)
            {
                arma::Mat<uint16_t> frame;
                inMovie->getFrame(frameIndex, frame);
                file.write((char*)frame.memptr(), frame.n_elem * sizeof(uint16_t));
            }
            else
            {
                MatrixFloat_t frame;
                inMovie->getFrame(frameIndex, frame);
                file.write((char*)frame.memptr(), frame.n_elem * sizeof(float));
            }
        }
    }

    template<typename T>
    void constructPatch(
        const std::string inFilename,
        const size_t inNumRows,
        const size_t inNumCols,
        const size_t inNumFrames,
        const std::tuple<size_t,size_t,size_t,size_t> & inRoi,
        CubeFloat_t & outPatch)
    {
        std::error_code error;
        mio::shared_mmap_source mmap;
        mmap.map(inFilename, error);
        if (error)
        {
            const std::string errorMessage = "Failed to memory map movie: " + error.message();
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        size_t numBytes = (inNumRows * inNumCols * inNumFrames * sizeof(T));
        if (size_t(mmap.size()) != numBytes)
        {
            const std::string errorMessage = "Failed memory mapped file read. Size of file (" + std::to_string(size_t(mmap.size())) + ") does not match size of movie (" + std::to_string(numBytes) + ")"; 
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        size_t rowStart = std::get<0>(inRoi);
        size_t rowEnd = std::get<1>(inRoi);
        size_t colStart = std::get<2>(inRoi);
        size_t colEnd = std::get<3>(inRoi);

        size_t patchRows = rowEnd - rowStart + 1;
        size_t patchCols = colEnd - colStart + 1;
        outPatch.set_size(patchRows, patchCols, inNumFrames);

        size_t index = 0;
        T * ptr = (T *)mmap.data();
        for (size_t t = 0; t < inNumFrames; t++)
        {
            // Initial ptr offset - point to first element of patch in current frame
            ptr += (inNumRows * colStart) + rowStart;

            for (size_t col = colStart; col <= colEnd; col++)
            {
                for (size_t row = rowStart; row <= rowEnd; row++)
                {
                    outPatch.at(index) = float(*ptr);
                    index++;
                    ptr++;
                }

                if (col == colEnd)
                {
                    // End of frame offset - point to first element of next frame
                    ptr += (inNumRows - (rowEnd + 1)) + (inNumRows * (inNumCols - (colEnd + 1)));
                }
                else
                {
                    // Per-column ptr offset - point to first element of next column in patch
                    ptr += (inNumRows - (rowEnd + 1)) + rowStart;
                }
            }
        }

    }

    void readMemoryMappedFileMovie(
        const std::string inFilename,
        const size_t inNumRows,
        const size_t inNumCols,
        const size_t inNumFrames,
        const DataType inDataType,
        const std::tuple<size_t,size_t,size_t,size_t> & inRoi,
        CubeFloat_t & outPatch)
    {
        // Validate ROI is within dimensions of input movie
        if (std::get<0>(inRoi) >= inNumRows
            || std::get<1>(inRoi) >=inNumRows
            || std::get<2>(inRoi) >= inNumCols
            || std::get<3>(inRoi) >= inNumCols)
        {
            const std::string errorMessage = "Failed memory mapped file read. Roi(" + std::to_string(std::get<0>(inRoi)) + ", " + std::to_string(std::get<1>(inRoi)) + ", " + std::to_string(std::get<2>(inRoi)) + ", " + std::to_string(std::get<3>(inRoi)) + ") out of range of input movie.";
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        // Validate ROI ranges are in increasing order
        if (std::get<0>(inRoi) >= std::get<1>(inRoi)
            || std::get<2>(inRoi) >= std::get<3>(inRoi))
        {
            const std::string errorMessage = "Failed memory mapped file read. Roi(" + std::to_string(std::get<0>(inRoi)) + ", " + std::to_string(std::get<1>(inRoi)) + ", " + std::to_string(std::get<2>(inRoi)) + ", " + std::to_string(std::get<3>(inRoi)) + ") range is non-increasing.";
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }
        
        if (inDataType == DataType::U16)
        {
            constructPatch<uint16_t>(inFilename, inNumRows, inNumCols, inNumFrames, inRoi, outPatch);
        }
        else if (inDataType == DataType::F32)
        {
            constructPatch<float>(inFilename, inNumRows, inNumCols, inNumFrames, inRoi, outPatch);
        }
        else
        {
            const std::string errorMessage = "Failed memory mapped file read. No conversion specified from data type (" + std::to_string(int(inDataType)) + ") to float.";
            ISX_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }
    }
} // namespace isx
