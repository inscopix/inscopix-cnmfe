#ifndef ISX_TEST_H
#define ISX_TEST_H

#include "isxArmaUtils.h"

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
/// \param  inActual    The actual value.
/// \param  inExpected  The expected value.
/// \param  inRelTol    The relative difference allowed for approximate equality.
///                     If 0, the values must be exactly equal.
/// \return             True if the actual value is approximately equal to the expected value.
void saveCubeToTiffFile(
    const isx::CubeFloat_t & inputData,
    const std::string & outputFilename);

#endif // ISX_TEST_H
