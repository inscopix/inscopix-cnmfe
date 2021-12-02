#ifndef ISX_TEST_H
#define ISX_TEST_H

#include "isxArmaUtils.h"
#include "isxExportTiff.h"

/// Check for approximate equality between two doubles
///
/// \param  inActual    The actual value.
/// \param  inExpected  The expected value.
/// \param  inRelTol    The relative difference allowed for approximate equality.
///                     If 0, the values must be exactly equal.
/// \return             True if the actual value is approximately equal to the expected value.
bool
approxEqual(
    const double inActual,
    const double inExpected,
    const double inRelTol);

/// Save an armadillo cube to a tiff stack file
///
/// \param  inputData       Input data in the form of an Armadillo Cube
/// \param  outputFilename  Path to the output tiff file.
template <class T>
void saveCubeToTiffFile(
    const arma::Cube<T> & inputData,
    const std::string & outputFilename)
{
    std::unique_ptr<isx::TiffExporter> out(new isx::TiffExporter(outputFilename, true));
    for (size_t i = 0; i < inputData.n_slices; ++i)
    {
        out->toTiffOut(inputData.slice(i));
        out->nextTiffDir();
    }
}

#endif // ISX_TEST_H
