#ifndef ISX_EXPORT_TIFF_H
#define ISX_EXPORT_TIFF_H

#include "isxArmaUtils.h"
#include "isxTiffBuffer.h"
#include "isxLog.h"
namespace libtiff {
    // placed in its own namespace to avoid
    // type redefinition conflict with OpenCV
    #include "tiffio.h"
}

namespace isx
{
    /// class that defines TiffExporter that wraps TIFF file
    class TiffExporter
    {
    public:
        /// constructor
        ///
        /// \param inFileName   out file path
        /// \param inBigTiff    if true, write to the BigTIFF format, otherwise don't
        TiffExporter(const std::string & inFileName, const bool inBigTiff = false);

        /// destructor
        ///
        ~TiffExporter();

        /// writes image to current Tiff directory
        ///
        /// \param inImage       pointer to image matrix
        template<typename T>
        void toTiffOut(const arma::Mat<T> & inImage)
        {
            // transpose image to account for armadillo column-major ordering
            // accordingly references to rows and columns are reversed below
            const arma::Mat<T> imageTrans = inImage.t();
            uint32_t width = uint32_t(imageTrans.n_rows);
            uint32_t height = uint32_t(imageTrans.n_cols);

            uint16_t sampleFormat;
            if (std::is_same<T, float>::value)
            {
                sampleFormat = SAMPLEFORMAT_IEEEFP;
            }
            else if (std::is_same<T, uint16_t>::value)
            {
                sampleFormat = SAMPLEFORMAT_UINT;
            }
            else
            {
                ISX_LOG_ERROR("Input datatype not supported. Only F32 and U16 are supported.");
                throw std::runtime_error("Input datatype not supported. Only F32 and U16 are supported.");
            }

            uint16_t bitsPerSample = uint16_t(sizeof(T) * 8);

            libtiff::TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);                    // set the width of the image
            libtiff::TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);                  // set the height of the image
            libtiff::TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);                   // set number of channels per pixel
            libtiff::TIFFSetField(out, TIFFTAG_SAMPLEFORMAT, sampleFormat);           // how to interpret each data sample in a pixel
            libtiff::TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);         // set the size of the channels
            libtiff::TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);     // set the origin of the image.
            libtiff::TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);    // set how the components of each pixel are stored (i.e. RGBRGBRGB or R plane, then G plane, then B plane)
            libtiff::TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);  // set the color space of the image data

            // The number of bytes between column 0 of two consecutive rows.
            size_t linebytes = sizeof(T) * imageTrans.n_rows;

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
                    ISX_LOG_ERROR("TiffExporter: error writing to output file");
                    throw std::runtime_error("Error writing to output file.");
                }
            }
        }

        /// switch to next TIFF Directory/Frame
        ///
        void nextTiffDir();
    private:
        /// The opaque TIFF directory struct used by libtiff.
        libtiff::TIFF * out;

        /// The offset of the last TIFF directory written.
        /// Note that uint64 is a typedef defined by libtiff and comes from including tiffio.
        // libtiff::uint64 lastOffDir = 0;
    };
} // namespace isx

#endif // ISX_EXPORT_TIFF_H
