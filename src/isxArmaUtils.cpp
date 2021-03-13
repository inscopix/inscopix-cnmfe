#define _USE_MATH_DEFINES
#include "isxArmaUtils.h"


namespace isx
{
    CubeFloat_t matrixToCubeByCol(const MatrixFloat_t & inMatrix, size_t inNRows, size_t inNCols)
    {
        CubeFloat_t cube(inNRows, inNCols, inMatrix.n_cols);
        for (size_t colIdx = 0; colIdx < inMatrix.n_cols; ++colIdx)
        {
            cube.slice(colIdx) = arma::reshape(inMatrix.col(colIdx), inNRows, inNCols);
        }
        return cube;
    }

    MatrixFloat_t cubeToMatrixBySlice(const CubeFloat_t & cube, bool colOrder)
    {
        MatrixFloat_t matrix(cube.n_rows * cube.n_cols, cube.n_slices);
        for (size_t sliceIdx = 0; sliceIdx < cube.n_slices; sliceIdx++)
        {
            matrix.col(sliceIdx) = arma::vectorise(colOrder ? cube.slice(sliceIdx) : cube.slice(sliceIdx).t());
        }
        return matrix;
    }

    float armaDotProduct(const ColumnFloat_t & u, const ColumnFloat_t & v)
    {
        ColumnFloat_t tmp = u % v;
        return arma::sum(tmp);
    }
} // namespace isx
