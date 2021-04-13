#ifndef ISX_TIFF_MOVIE
#define ISX_TIFF_MOVIE

#include "isxArmaUtils.h"
#include "isxUtilities.h"
#include <string>

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

        /// Constructor that allows specification of the number of directories
        /// to avoid expensive IO to get this.
        /// \param inFileName the filename for one TIFF movie file
        /// \param inNumDirectories the number of directories in the TIFF movie file
        TiffMovie(const std::string & inFileName, const size_t inNumDirectories);

        /// Destructor
        ///
        ~TiffMovie();

        /// Get a movie frame
        /// \param inFrameNumber frame index
        /// \param frame         output frame
        /// \throw  isx::ExceptionDataIO    If inFrameNumber is out of range.
        void getFrame(size_t inFrameNumber, MatrixFloat_t & frame);

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
}

#endif //ISX_TIFF_MOVIE
