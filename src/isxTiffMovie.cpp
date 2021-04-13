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
    TiffMovie::TiffMovie(const std::string & inFileName, const size_t inNumDirectories)
    {
        initialize(inFileName);
        m_numFrames = inNumDirectories;
    }

    TiffMovie::TiffMovie(const std::string & inFileName)
    {
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
            throw std::runtime_error("Failed to open TIFF file: " +  m_fileName);
        }

        // This check is mainly to catch multi-channel files exported by ImageJ (e.g. RGB 8-bit).
        uint16_t channels = 0;
        libtiff::TIFFGetField(m_tif, TIFFTAG_SAMPLESPERPIXEL, &channels);
        if (channels > 1)
        {
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
            default:
            {
                throw std::runtime_error("Unsupported number of bits (" + std::to_string(bits) + "). Only 16 (U16) and 32 (F32) bit images are supported.");
            }
        }

        uint32_t width, height;
        libtiff::TIFFGetField(m_tif, TIFFTAG_IMAGEWIDTH, &width);
        libtiff::TIFFGetField(m_tif, TIFFTAG_IMAGELENGTH, &height);

        m_frameWidth = size_t(width);
        m_frameHeight = size_t(height);
    }

    void
    TiffMovie::getFrame(size_t inFrameNumber, MatrixFloat_t & frame)
    {
        // Seek to the right directory
        if(1 != libtiff::TIFFSetDirectory(m_tif, libtiff::tdir_t(inFrameNumber)))
        {
            throw std::runtime_error("The requested frame number doesn't exist.");
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
                throw std::runtime_error("Failed to read strip from TIFF file: " + m_fileName);
            }
            pBuf += size;
        }

        // Copy the data to the frame buffer
        frame = arma::Mat<float>(m_frameWidth, m_frameHeight);
        std::memcpy(frame.memptr(), (float *)buf.get(), nbytes);
        frame = frame.t();
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
