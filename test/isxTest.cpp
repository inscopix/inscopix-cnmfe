#include "isxTest.h"
#include "isxExportTiff.h"
#include "catch.hpp"

bool
approxEqual(
    const double inActual,
    const double inExpected,
    const double inRelTol)
{
    if (inRelTol > 0)
    {
        return inActual == Approx(inExpected).epsilon(inRelTol);
    }
    return inActual == inExpected;
}

void saveCubeToTiffFile(
    const isx::CubeFloat_t & inputData,
    const std::string & outputFilename)
{
    std::unique_ptr<isx::TiffExporter> out(new isx::TiffExporter(outputFilename, true));
    for (size_t i = 0; i < inputData.n_slices; ++i)
    {
        out->toTiffOut(inputData.slice(i));
        out->nextTiffDir();
    }
}
