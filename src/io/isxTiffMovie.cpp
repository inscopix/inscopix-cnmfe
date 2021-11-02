#include "isxTiffMovie.h"
#include "isxTiffBuffer.h"
namespace libtiff {
    // placed in its own namespace to avoid
    // type redefinition conflict with OpenCV
    #include "tiffio.h"
}
#include <cstring>

namespace isx
{
    TiffMovie::TiffMovie(const std::string & inFileName)
    {
        ISX_LOG_INFO("Loading tiff input (file: ", inFileName, ")");
        initialize(inFileName);
        m_numFrames = size_t(libtiff::TIFFNumberOfDirectories(m_tif));
    }

    TiffMovie::~TiffMovie()
    {
        libtiff::TIFFClose(m_tif);
    }

    void
    TiffMovie::initialize(const std::string & inFileName)
    {
        m_fileName = inFileName;

        m_tif = libtiff::TIFFOpen(inFileName.c_str(), "r");

        if(!m_tif)
        {
            ISX_LOG_ERROR("Failed to open TIFF file: " +  m_fileName);
            throw std::runtime_error("Failed to open TIFF file: " +  m_fileName);
        }

        // This check is mainly to catch multi-channel files exported by ImageJ (e.g. RGB 8-bit).
        uint16_t channels = 0;
        libtiff::TIFFGetField(m_tif, TIFFTAG_SAMPLESPERPIXEL, &channels);
        if (channels > 1)
        {
            ISX_LOG_ERROR("Unsupported number of channels (" + std::to_string(channels) + "). Only single channel TIFF images are supported.");
            throw std::runtime_error("Unsupported number of channels (" + std::to_string(channels) + "). Only single channel TIFF images are supported.");
        }

        uint16_t bits;
        libtiff::TIFFGetField(m_tif, TIFFTAG_BITSPERSAMPLE, &bits);
        switch (bits)
        {
            case sizeof(float) * 8:
            {
                m_dataType = DataType::F32;
                break;
            }
            case sizeof(uint16_t) * 8:
            {
                m_dataType = DataType::U16;
                break;
            }
            case sizeof(uint8_t) * 8:
            {
                m_dataType = DataType::U8;
                break;
            }
            default:
            {
                ISX_LOG_ERROR("Unsupported number of bits (" + std::to_string(bits) + "). Only 8 (U8), 16 (U16), and 32 (F32) bit images are supported.");
                throw std::runtime_error("Unsupported number of bits (" + std::to_string(bits) + "). Only 8 (U8), 16 (U16), and 32 (F32) bit images are supported.");
            }
        }

        uint32_t width, height;
        libtiff::TIFFGetField(m_tif, TIFFTAG_IMAGEWIDTH, &width);
        libtiff::TIFFGetField(m_tif, TIFFTAG_IMAGELENGTH, &height);

        m_frameWidth = size_t(width);
        m_frameHeight = size_t(height);
    }

    void
    TiffMovie::getFrameBytes(size_t inFrameNumber, arma::Col<char> & outBuffer)
    {
        // Seek to the right directory
        if(1 != libtiff::TIFFSetDirectory(m_tif, libtiff::tdir_t(inFrameNumber)))
        {
            ISX_LOG_ERROR("Invalid frame number: the requested frame number does not exist");
            throw std::invalid_argument("The requested frame number doesn't exist.");
        }

        // Read the image
        libtiff::tsize_t size = libtiff::TIFFStripSize(m_tif);

        size_t nbytes = m_frameWidth * m_frameHeight * getDataTypeSizeInBytes(m_dataType);
        TIFFBuffer buf(nbytes);
        char * pBuf = (char *)buf.get();

        auto numOfStrips = libtiff::TIFFNumberOfStrips(m_tif);
        for (libtiff::tstrip_t strip = 0; strip < numOfStrips; strip++)
        {
            if (libtiff::TIFFReadEncodedStrip(m_tif, strip, pBuf, size) == -1)
            {
                ISX_LOG_ERROR("Failed to read strip from TIFF file: " + m_fileName);
                throw std::runtime_error("Failed to read strip from TIFF file: " + m_fileName);
            }
            pBuf += size;
        }

        outBuffer.set_size(nbytes);
        std::memcpy(outBuffer.memptr(), (char *)buf.get(), nbytes);
    }

    size_t
    TiffMovie::getNumFrames() const
    {
        return m_numFrames;
    }

    size_t
    TiffMovie::getFrameWidth() const
    {
        return m_frameWidth;
    }

    size_t
    TiffMovie::getFrameHeight() const
    {
        return m_frameHeight;
    }

    DataType
    TiffMovie::getDataType() const
    {
        return m_dataType;
    }
}
