#include "isxUtilities.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>

namespace isx
{
    size_t getDataTypeSizeInBytes(DataType inDataType)
    {
        switch (inDataType)
        {
            case DataType::F32:
            {
                return sizeof(float);
            }
            case DataType::U16:
            {
                return sizeof(uint16_t);
            }
            case DataType::U8:
            {
                return sizeof(uint8_t);
            }
            default:
            {
                return 0;
            }
        }
    }

    void removeFiles(const std::vector<std::string> & inFilePaths)
    {
        for (const auto & f : inFilePaths)
        {
            std::remove(f.c_str());
        }
    }

    bool pathExists (const std::string & filename)
    {
        struct stat buffer;
        return (stat (filename.c_str(), &buffer) == 0);
    }

    bool makeDirectory(const std::string & path)
    {
        if (mkdir(path.c_str(), 0777) == -1)
        {
            return false;
        }
        return true;
    }

    bool removeDirectory(const std::string & path)
    {
        if (rmdir(path.c_str()) == -1)
        {
            return false;
        }
        return true;
    }

    std::string getBaseName(const std::string & path)
    {
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);
        return filename.substr(0, filename.find_last_of('.'));
    }
}
