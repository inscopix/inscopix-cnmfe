#include "isxTest.h"
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
