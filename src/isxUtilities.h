#ifndef ISX_UTILITIES
#define ISX_UTILITIES

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>

namespace isx
{
    /// The data types of values.
    enum class DataType
    {
        F32 = 0,
        U16,
        U8
    };

    /// Get the size of a data type in bytes.
    ///
    /// \param  inDataType  The data type.
    /// \return             The size of the given data type in bytes.
    size_t getDataTypeSizeInBytes(DataType inDataType);

    /// Delete files from disk
    /// \param inFilePaths      Paths of the files to remove
    void removeFiles(const std::vector<std::string> & inFilePaths);

    /// Checks whether a file exists
    /// Limitation: only checks for file accessibility, if the file is locked by another program or
    ///             if there is no access to the file it will return False
    ///
    /// \param filename
    /// \return
    bool fileExists (const std::string & filename);
} // namespace

#endif //ISX_TIFF_MOVIE
