#include "isxMemoryMappedFileUtils.h"

#include <QFile>

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
            throw std::runtime_error("Failed memory mapped file write. No conversion specified from data type to float");
        }

        QFile file(QString::fromStdString(inFilename));
        if (!file.open(QIODevice::ReadWrite))
        {
            throw std::runtime_error("Failed memory mapped file write. Unable to open new file.");
        }

        if (!file.resize(numBytes))
        {
            throw std::runtime_error("Failed memory mapped file write. Unable to resize new file.");
        }

        uchar * map = file.map(0, file.size());
        if (map == nullptr)
        {
            throw std::runtime_error("Failed memory mapped file write. Unable to map new file into memory.");
        }

        for (size_t frameIndex = 0; frameIndex < numFrames; frameIndex++)
        {
            MatrixFloat_t frame;
            inMovie->getFrame(frameIndex, frame);

            if (dataType == DataType::U16)
            {
                file.write((char*)frame.memptr(), frame.n_elem * sizeof(uint16_t));
            }
            else
            {
                file.write((char*)frame.memptr(), frame.n_elem * sizeof(float));
            }
        }

        if (!file.unmap(map))
        {
            throw std::runtime_error("Failed memory mapped file write. Unable to unmap new file from memory.");
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
        QFile file(QString::fromStdString(inFilename));
        if (!file.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error("Failed memory mapped file read. Unable to open file.");
        }

        size_t numBytes = (inNumRows * inNumCols * inNumFrames * sizeof(T));
        if (size_t(file.size()) != numBytes)
        {
            throw std::runtime_error("Failed memory mapped file read. Size of file does not match size of movie");
        }

        uchar * map = file.map(0, file.size());
        if (map == nullptr)
        {
            throw std::runtime_error("Failed memory mapped file read. Unable to map file into memory.");
        }

        size_t rowStart = std::get<0>(inRoi);
        size_t rowEnd = std::get<1>(inRoi);
        size_t colStart = std::get<2>(inRoi);
        size_t colEnd = std::get<3>(inRoi);

        size_t patchRows = rowEnd - rowStart + 1;
        size_t patchCols = colEnd - colStart + 1;
        outPatch.set_size(patchRows, patchCols, inNumFrames);

        size_t index = 0;
        T * ptr = (T *)map;
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

        if (!file.unmap(map))
        {
            throw std::runtime_error("Failed memory mapped file read. Unable to unmap file from memory.");
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
        // Check inputs, ensure nothing is obviously wrong with them
        {
            if (std::get<0>(inRoi) >= inNumRows
                || std::get<1>(inRoi) >=inNumRows
                || std::get<2>(inRoi) >= inNumCols
                || std::get<3>(inRoi) >= inNumCols)
            {
                throw std::runtime_error("Failed memory mapped file read. ROI out of range of input movie");
            }

            if (std::get<0>(inRoi) >= std::get<1>(inRoi)
                || std::get<2>(inRoi) >= std::get<3>(inRoi))
            {
                throw std::runtime_error("Failed memory mapped file read. ROI is not in the correct form");
            }
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
            throw std::runtime_error("Failed memory mapped file read. No conversion specified from data type to float");
        }
    }
} // namespace isx
