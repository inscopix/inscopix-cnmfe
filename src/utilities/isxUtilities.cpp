#include "isxUtilities.h"
#include <sys/stat.h>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <direct.h>  // Windows
#else
#include <unistd.h>  // Mac & Linux
#endif

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

    std::string
    getFileName(const std::string & inPath)
    {
        return inPath.substr(inPath.find_last_of("/\\") + 1);
    }

    std::string
    getDirName(const std::string & inPath)
    {
        return inPath.substr(0, inPath.find_last_of("/\\"));
    }

    void removeFiles(const std::vector<std::string> & inFilePaths)
    {
        for (const auto & f : inFilePaths)
        {
            std::remove(f.c_str());
        }
    }

    bool pathExists(const std::string & filename)
    {
        struct stat buffer;
        return (stat (filename.c_str(), &buffer) == 0);
    }

    bool makeDirectory(const std::string & path)
    {
		int status;
		#ifdef _WIN32
		status = mkdir(path.c_str());  // Windows
		#else
		status = mkdir(path.c_str(), 0777);  // Mac & Linux
		#endif

		if (status == -1)
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

    std::string getCurrentDateTime(const std::string & format, const bool includeMilliseconds) {
        using namespace std::chrono;
        system_clock::time_point now = system_clock::now();

        time_t timer = system_clock::to_time_t(now);
        std::ostringstream oss;
        char timeStr[20];
        if (std::strftime(timeStr, sizeof(timeStr), format.c_str(), std::localtime(&timer)))
        {
            oss << timeStr; // HH:MM:SS
        }

        if (includeMilliseconds)
        {
            milliseconds ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        }
        
        return oss.str();
    }
}
