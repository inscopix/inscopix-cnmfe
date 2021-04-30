#ifndef ISX_TIFF_BUFFER_H
#define ISX_TIFF_BUFFER_H

#include <stddef.h>

namespace isx 
{
    /// A class to manage a buffer allocated through the LibTiff library
    class TIFFBuffer
    {
    public:
        /// Constructor
        /// \param inBytes  size of the buffer in bytes
        TIFFBuffer(size_t inBytes);

        TIFFBuffer(const TIFFBuffer &) = delete;

        TIFFBuffer & operator=(const TIFFBuffer &) = delete;

        /// Destructor
        ///
        ~TIFFBuffer();

        /// Get pointer to the buffer memory
        ///
        void * get() const;

    private:
        void * m_buf = nullptr;
    };
}

#endif // ISX_TIFF_BUFFER_H
