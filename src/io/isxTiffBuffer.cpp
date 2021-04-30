#include "isxTiffBuffer.h"
namespace libtiff {
    // placed in its own namespace to avoid
    // type redefinition conflict with OpenCV
    #include "tiffio.h"
}

namespace isx
{
    TIFFBuffer::TIFFBuffer(size_t inBytes)
    {
        m_buf = libtiff::_TIFFmalloc(libtiff::tsize_t(inBytes));
    }

    TIFFBuffer::~TIFFBuffer()
    {
        if (m_buf != nullptr)
        {
            libtiff::_TIFFfree(m_buf);
        }
    }

    void * TIFFBuffer::get() const
    {
        return m_buf;
    }
}
