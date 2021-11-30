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

    /// Get the file name of a path.
    ///
    /// The file name does not include the directory name, but does include the
    /// extension.
    /// This name is perhaps misleading because this will work for
    /// directories, but in that case should act as getBaseName.
    ///
    /// \param  inPath  The path from which to get the file name.
    /// \return         The file name of the path.
    std::string getFileName(const std::string & inPath);

    /// Get the directory name of a path.
    ///
    /// \param  inPath  The path from which to get the base name.
    /// \return         The directory name of the path.
    std::string getDirName(const std::string & inPath);

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

    /// Removes a directory and all its content using the specified path
    ///
    /// \param path
    /// \return   True if the directory was successfully removed
    bool removeDirectory(const std::string & path);

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

    /// Get current date & time
    /// Format is YYYY-MM-DD HH:mm:ss.xxx (xxx represents milliseconds)
    ///
    /// \param  includeMilliseconds     Appends milliseconds to string
    /// \return                         Current date time as a string
    std::string getCurrentDateTime(const std::string & format, const bool includeMilliseconds=true);

    /// Get package version number
    ///
    /// \param  versionFilePath     Path to the text file containing the version
    /// \return       version number (e.g. "1.0.0")
    std::string getPackageVersion(std::string versionFilePath = "version.txt");

} // namespace

#endif //ISX_TIFF_MOVIE
