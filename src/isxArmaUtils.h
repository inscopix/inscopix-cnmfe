#ifndef ISX_ARMA_UTILS_H
#define ISX_ARMA_UTILS_H
#define ARMA_USE_CXX11

#include <armadillo>
#include <opencv2/core/core.hpp>

namespace isx
{

/// A column vector storing float values.
typedef arma::Col<float> ColumnFloat_t;

/// A row vector storing float values.
typedef arma::Row<float> RowFloat_t;

// A matrix storing float values.
typedef arma::Mat<float> MatrixFloat_t;

// A cube storing float values.
typedef arma::Cube<float> CubeFloat_t;

/// Converts an Armadillo matrix to a cube, converting each column to a slice in output cube
///
/// \param inMatrix             input
/// \param inNRows              number of rows in each slice
/// \param inNCols              number of columns in each slice
CubeFloat_t matrixToCubeByCol(
    const MatrixFloat_t & inMatrix,
    size_t inNRows,
    size_t inNCols);

/// Converts an Armadillo cube to a matrix, converting each slice to a column in output matrix
///
/// \param inCube               input
/// \param colOrder             interpret values in column as column major ordering. otherwise, as row major
MatrixFloat_t cubeToMatrixBySlice(
    const CubeFloat_t & cube,
    bool colOrder = true);

/// Convert an Armadillo matrix to an OpenCV matrix.
///
/// \param  inSrc           Armadillo matrix of type Src.
/// \param  inTranspose     If true, transpose matrix to get same representation (due to differing memory layouts).
/// \return                 OpenCV matrix of type Dst.
template <typename Src, typename Dst = Src>
inline cv::Mat_<Dst>
armaToCvMat(const arma::Mat<Src> & inSrc, bool inTranspose = true);

/// Convert an OpenCV matrix to an Armadillo matrix.
///
/// \param  inSrc           OpenCV matrix of type Src.
/// \param  inTranspose     If true, transpose matrix to get same representation (due to differing memory layouts).
/// \return                 Armadillo matrix of type Dst.
template <typename Src, typename Dst = Src>
inline arma::Mat<Dst>
cvToArmaMat(const cv::Mat_<Src> & inSrc, bool inTranspose = true);


// Templated Implementations

template <typename Src, typename Dst>
inline cv::Mat_<Dst>
armaToCvMat(const arma::Mat<Src> & inSrc, bool inTranspose)
{
    cv::Mat tmp = cv::Mat_<Src>{int(inSrc.n_cols), int(inSrc.n_rows), const_cast<Src*>(inSrc.memptr())};
    return inTranspose ? cv::Mat_<Dst>(tmp).t() : cv::Mat_<Dst>(tmp);
}

template <typename Src, typename Dst>
inline arma::Mat<Dst>
cvToArmaMat(const cv::Mat_<Src> & inSrc, bool inTranspose)
{
    arma::Mat<Src> tmp(inSrc.cols, inSrc.rows);
    inSrc.copyTo(cv::Mat_<Src>{inSrc.rows, inSrc.cols, tmp.memptr()});
    return inTranspose ? arma::conv_to<arma::Mat<Dst>>::from(tmp).t() : arma::conv_to<arma::Mat<Dst>>::from(tmp);
}

} // namespace isx

#endif // ISX_ARMA_UTILS_H
