#ifndef ISX_TIFF_MOVIE
#define ISX_TIFF_MOVIE

#include "isxArmaUtils.h"
#include "isxUtilities.h"
#include "isxLog.h"
#include <string>
#include <memory>

/// Forward-declare TIFF formats
struct tiff;
typedef struct tiff TIFF;

namespace isx 
{
    /// A class managing a single TIFF file
    ///
    class TiffMovie
    {
    public: 

        /// Constructor
        /// \param inFileName the filename for one TIFF movie file
        TiffMovie(const std::string & inFileName);

        /// Destructor
        ///
        ~TiffMovie();

        /// Get a movie frame (as bytes)
        /// \param inFrameNumber frame index
        /// \param outBuffer     buffer storing bytes of frame
        /// \throw  isx::ExceptionDataIO    If inFrameNumber is out of range.
        void getFrameBytes(size_t inFrameNumber, arma::Col<char> & outBuffer);

        /// Get a movie frame
        /// \param inFrameNumber frame index
        /// \param frame         output frame
        /// \throw  isx::ExceptionDataIO    If inFrameNumber is out of range.
        template<typename T>
        void getFrame(size_t inFrameNumber, arma::Mat<T> & frame);

        /// \return the total number of frames in the movie
        ///
        size_t
        getNumFrames() const;

        /// return the width in pixels
        ///
        size_t
        getFrameWidth() const;

        /// \return the height in pixels
        ///
        size_t
        getFrameHeight() const;

        /// return the data type of pixels
        ///
        DataType
        getDataType() const;

    private:

        void initialize(const std::string & inFileName);

        std::string m_fileName;
        tiff *      m_tif;

        size_t m_frameWidth;
        size_t m_frameHeight;
        size_t m_numFrames;
        DataType m_dataType;
    };

    using SpTiffMovie_t = std::shared_ptr<TiffMovie>;


    template<typename T>
    void
    TiffMovie::getFrame(size_t inFrameNumber, arma::Mat<T> & frame)
    {
        arma::Col<char> buf;
        getFrameBytes(inFrameNumber, buf);
        size_t nbytes = m_frameWidth * m_frameHeight * getDataTypeSizeInBytes(m_dataType);

        // Copy the data to the frame buffer
        if (m_dataType == DataType::F32)
        {
            if (std::is_same<T, float>::value)
            {
                frame = arma::Mat<T>(m_frameWidth, m_frameHeight);
                std::memcpy(frame.memptr(), (T *)buf.memptr(), nbytes);
            }
            else
            {
                arma::Mat<float> tmpFrame(m_frameWidth, m_frameHeight);
                std::memcpy(tmpFrame.memptr(), (float *)buf.memptr(), nbytes);
                frame = arma::conv_to<arma::Mat<T>>::from(tmpFrame);
            }
        }
        else if (m_dataType == DataType::U16)
        {
            if (std::is_same<T, uint16_t>::value)
            {
                frame = arma::Mat<T>(m_frameWidth, m_frameHeight);
                std::memcpy(frame.memptr(), (T *)buf.memptr(), nbytes);
            }
            else
            {
                arma::Mat<uint16_t> tmpFrame(m_frameWidth, m_frameHeight);
                std::memcpy(tmpFrame.memptr(), (uint16_t *)buf.memptr(), nbytes);
                frame = arma::conv_to<arma::Mat<T>>::from(tmpFrame);
            }

        }
        else if (m_dataType == DataType::U8)
        {
            if (std::is_same<T, uint8_t>::value)
            {
                frame = arma::Mat<T>(m_frameWidth, m_frameHeight);
                std::memcpy(frame.memptr(), (T *)buf.memptr(), nbytes);
            }
            else
            {
                arma::Mat<uint8_t> tmpFrame(m_frameWidth, m_frameHeight);
                std::memcpy(tmpFrame.memptr(), (uint8_t *)buf.memptr(), nbytes);
                frame = arma::conv_to<arma::Mat<T>>::from(tmpFrame);
            }
        }
        else
        {
            ISX_LOG_ERROR("Tiff input datatype not supported. Only F32, U16, and U8 are supported.");
            throw std::runtime_error("Tiff input datatype not supported. Only F32, U16, and U8 are supported.");
        }

        frame = frame.t();
    }
}

#endif //ISX_TIFF_MOVIE
