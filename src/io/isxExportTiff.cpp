#include "isxExportTiff.h"

namespace isx
{
    TiffExporter::TiffExporter(const std::string & inFileName, const bool inBigTiff)
    {
        const char * mode = inBigTiff ? "w8" : "w";
        out = libtiff::TIFFOpen(inFileName.c_str(), mode);

        if (!out)
        {
            ISX_LOG_ERROR("TiffExporter: unable to open file for writing");
            throw std::runtime_error("Unable to open file for writing.");
        }
    }

    TiffExporter::~TiffExporter()
    {
        libtiff::TIFFClose(out);
    }

    void TiffExporter::nextTiffDir()
    {
        // libtiff::TIFFWriteDirectoryFast(out, lastOffDir, &lastOffDir);
        libtiff::TIFFWriteDirectory(out);
        libtiff::TIFFFlush(out);
    }
}
