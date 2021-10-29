#ifndef ISX_EXPORT_TIFF_H
#define ISX_EXPORT_TIFF_H

#include "isxArmaUtils.h"
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
        /// \param inImage       pointer to float image matrix
        void toTiffOut(const MatrixFloat_t & inImage);

        /// writes image to current Tiff directory
        ///
        /// \param inImage       pointer to uint16 image matrix
        void toTiffOut(const arma::Mat<uint16_t> & inImage);

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
