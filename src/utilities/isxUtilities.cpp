#include "isxUtilities.h"
#include <QFileInfo>
#include <QDir>


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
        QFileInfo pathInfo(QString::fromStdString(inPath));
        return pathInfo.fileName().toStdString();
    }

    std::string
    getDirName(const std::string & inPath)
    {
        QFileInfo pathInfo(QString::fromStdString(inPath));
        QDir pathDir = pathInfo.dir();
        return pathDir.path().toStdString();
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
        QFileInfo pathInfo(QString::fromStdString(filename));
        return pathInfo.exists();
    }

    bool makeDirectory(const std::string & path)
    {
        QDir dir(QString::fromStdString(getDirName(path)));
        return dir.mkdir(QString::fromStdString(getFileName(path)));
    }

    bool removeDirectory(const std::string & path)
    {
        QDir dir(QString::fromStdString(path));
        return dir.removeRecursively();
    }

    std::string getBaseName(const std::string & path)
    {
        QFileInfo pathInfo(QString::fromStdString(path));
        return pathInfo.baseName().toStdString();
    }
}
