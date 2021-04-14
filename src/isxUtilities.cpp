#include "isxUtilities.h"
#include <sys/stat.h>
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

    bool fileExists (const std::string & filename) {
        struct stat buffer;
        return (stat (filename.c_str(), &buffer) == 0);
    }
}
