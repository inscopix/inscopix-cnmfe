#ifndef ISX_UTILITIES
#define ISX_UTILITIES

#include <stddef.h>
#include <stdint.h>

namespace isx
{
    /// The data types of values.
    enum class DataType
    {
        U16 = 0,
        F32,
        U8,
        RGB888,
    };

    /// Get the size of a data type in bytes.
    ///
    /// \param  inDataType  The data type.
    /// \return             The size of the given data type in bytes.
    size_t getDataTypeSizeInBytes(DataType inDataType);
} // namespace

#endif //ISX_TIFF_MOVIE