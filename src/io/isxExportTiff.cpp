#include "isxExportTiff.h"
#include "isxTiffBuffer.h"

namespace isx
{
    TiffExporter::TiffExporter(const std::string & inFileName, const bool inBigTiff)
    {
        const char * mode = inBigTiff ? "w8" : "w";
        out = libtiff::TIFFOpen(inFileName.c_str(), mode);

        if (!out)
        {
            throw std::runtime_error("Unable to open file for writing.");
        }
    }

    TiffExporter::~TiffExporter()
    {
        libtiff::TIFFClose(out);
    }

    void TiffExporter::toTiffOut(const MatrixFloat_t & inImage)
    {
        // transpose image to account for armadillo column-major ordering
        // accordingly references to rows and columns are reversed below
        const MatrixFloat_t imageTrans = inImage.t();
        uint32_t width = uint32_t(imageTrans.n_rows);
        uint32_t height = uint32_t(imageTrans.n_cols);

        uint16_t sampleFormat = SAMPLEFORMAT_IEEEFP;  // 32-bit float
        uint16_t bitsPerSample = uint16_t(sizeof(float) * 8);

        libtiff::TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);                    // set the width of the image
        libtiff::TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);                  // set the height of the image
        libtiff::TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);                   // set number of channels per pixel
        libtiff::TIFFSetField(out, TIFFTAG_SAMPLEFORMAT, sampleFormat);           // how to interpret each data sample in a pixel
        libtiff::TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);         // set the size of the channels
        libtiff::TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);     // set the origin of the image.
        libtiff::TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);    // set how the components of each pixel are stored (i.e. RGBRGBRGB or R plane, then G plane, then B plane)
        libtiff::TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);  // set the color space of the image data

        // The number of bytes between column 0 of two consecutive rows.
        size_t linebytes = sizeof(float) * imageTrans.n_rows;

        // Allocating memory to store the pixels of current row
        TIFFBuffer buf(linebytes);

        // We set the strip size of the file to be size of one row of pixels
        libtiff::TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, 1);

        // The byte array used to store data.
        const char * pixels = reinterpret_cast<const char *>(imageTrans.memptr());

        for (uint32_t row = 0; row < height; row++)
        {
            std::memcpy(buf.get(), &pixels[row*linebytes], linebytes);

            if (libtiff::TIFFWriteScanline(out, buf.get(), row, 0) < 0)
            {
                throw std::runtime_error("Error writing to output file.");
            }
        }
    }

    void TiffExporter::nextTiffDir()
    {
        // libtiff::TIFFWriteDirectoryFast(out, lastOffDir, &lastOffDir);
        libtiff::TIFFWriteDirectory(out);
        libtiff::TIFFFlush(out);
    }
}
