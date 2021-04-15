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

    /// Checks whether a path exists (file or directory)
    /// Limitation: only checks for file accessibility, if the file is locked by another program or
    ///             if there is no access to the file it will return False
    ///
    /// \param filename
    /// \return  True if the path exists
    bool pathExists (const std::string & filename);

    /// Creates a new directory using the specified path
    ///
    /// \param path
    /// \return   True if the directory was successfully created
    bool makeDirectory(const std::string & path);

    /// Get the base name of a path
    ///
    /// The base name does not include the extension nor the directory name.
    ///
    /// For example:
    /// - "movie.tiff" -> "movie"
    /// - "outputs" -> "outputs"
    /// - "outputs/movie.tiff" -> "movie"
    /// - "outputs/day1" -> "day1"
    ///
    /// \param  path  The path from which to get the base name.
    /// \return         The base name of the path.
    std::string getBaseName(const std::string & path);

} // namespace

#endif //ISX_TIFF_MOVIE
