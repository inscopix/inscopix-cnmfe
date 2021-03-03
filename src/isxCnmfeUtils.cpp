#include "isxCnmfeUtils.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <map>

namespace isx
{
    uint32_t nextPowerOf2(const uint32_t n)
    {
        uint32_t p = 1;
        while (p < n) {
            p *= 2;
        }
        return p;
    }

    void autoCovariance(const ColumnFloat_t & inData, ColumnFloat_t & outXcov, uint32_t inMaxlag)
    {
        float mean = arma::mean(inData);
        ColumnFloat_t normVec = inData - mean;

        uint32_t bins = static_cast<uint32_t>(normVec.n_elem);

        // Resize vec to next pow of 2 elements, padding with 0s
        uint32_t outputSize = nextPowerOf2(2 * bins - 1);
        normVec.resize(outputSize);

        arma::cx_fvec fftVec = arma::fft(normVec);
        ColumnFloat_t smVec = arma::square(arma::abs(fftVec));
        arma::cx_fvec compVec(smVec, arma::zeros<ColumnFloat_t>(smVec.n_elem)); // Convert to complex vector
        arma::cx_fvec ifftVec = arma::ifft(compVec);
        arma::cx_fvec concatVec;
        if (inMaxlag == 0){
            concatVec = ifftVec(arma::span(0, inMaxlag));
        }
        else
        {
            concatVec = arma::join_cols(
                ifftVec(arma::span(outputSize - inMaxlag, outputSize - 1)), // last inMaxlag elements
                ifftVec(arma::span(0, inMaxlag))                                 // first inMaxlag + 1 elements
            );
        }

        outXcov = arma::real(concatVec / static_cast<float>(bins));
    }

    // helper function to construct a decimation matrix
    static MatrixFloat_t createDecimationMatrix(const std::pair<size_t,size_t> inDims, const size_t subsamplingFactor)
    {
        std::pair<size_t,size_t> inDimsDs(inDims);
        inDimsDs.first = (inDimsDs.first - 1)/subsamplingFactor + 1;
        inDimsDs.second = (inDimsDs.second - 1)/subsamplingFactor + 1;

        size_t D = inDimsDs.first * inDimsDs.second;
        MatrixFloat_t dsMatrix = arma::eye<MatrixFloat_t>(D,D);

        CubeFloat_t tmpCube = arma::zeros<CubeFloat_t>(inDimsDs.first * subsamplingFactor, inDimsDs.second * subsamplingFactor, D);
        size_t rowIdx = 0;
        size_t colIdx = 0;

        for (size_t sliceIdx = 0; sliceIdx < tmpCube.n_slices; sliceIdx++)
        {
            tmpCube.slice(sliceIdx).submat(
                arma::span(rowIdx, rowIdx + subsamplingFactor - 1),
                arma::span(colIdx, colIdx + subsamplingFactor - 1)
            ).ones();

            rowIdx += subsamplingFactor;
            if (rowIdx >= tmpCube.n_rows)
            {
                colIdx += subsamplingFactor;
                rowIdx = 0;
            }
        }

        tmpCube = tmpCube(arma::span(0, inDims.first - 1), arma::span(0, inDims.second - 1), arma::span::all);

        MatrixFloat_t tmpMat = arma::zeros<MatrixFloat_t>(tmpCube.n_slices, tmpCube.n_rows * tmpCube.n_cols);
        for (size_t sliceIdx = 0; sliceIdx < tmpCube.n_slices; sliceIdx++)
        {
            tmpMat.row(sliceIdx) = arma::vectorise(tmpCube.slice(sliceIdx)).t();
        }

        ColumnFloat_t sumRows = arma::sum(tmpMat, 1);
        tmpMat.each_col() /= sumRows;
        return tmpMat;
    }

    arma::SpMat<float> generateDecimationMatrix(const std::pair<size_t,size_t> inDims, const size_t subsamplingFactor)
    {
        // create decimation matrix
        MatrixFloat_t tmpDecMat = createDecimationMatrix(
            std::pair<size_t,size_t>(inDims.first, subsamplingFactor), subsamplingFactor);

        // row indices
        size_t D = inDims.first * inDims.second;

        arma::ivec tmpInd(tmpDecMat.n_cols);
        for (size_t i = 0; i < tmpDecMat.n_cols; i++)
        {
            arma::uvec loc = arma::find(tmpDecMat.col(i));
            tmpInd(i) = loc(0);
        }

        arma::ivec ind(tmpInd);
        for (size_t i = 0; i < inDims.second/subsamplingFactor; i++)
        {
            ind = arma::join_cols(ind, tmpInd);
        }

        arma::ivec ind2 = arma::regspace<arma::ivec>(0, D - 1);
        ind2 = ind2 / (inDims.first * subsamplingFactor) * ((inDims.first - 1) / subsamplingFactor + 1);
        ind = ind(arma::span(0, D-1)) + ind2;

        // count unique values
        std::map<int,int> counts;
        ind.for_each([&counts](int val){ counts[val]++; });

        // replace vals with respective counts
        ColumnFloat_t values(ind.size());
        for (size_t i = 0; i < ind.size(); i++)
        {
            values(i) = 1.0f / static_cast<float>(counts[static_cast<int>(ind(i))]);
        }

        // construct sparse matrix
        arma::uvec colptr = arma::conv_to<arma::uvec>::from(arma::regspace<arma::ivec>(0, D));
        arma::uvec rowind = arma::conv_to<arma::uvec>::from(ind);
        arma::SpMat<float> decMat(rowind, colptr, values, counts.size(), D);
        return decMat;
    }

    std::pair<float, float> computeCentroid(const MatrixFloat_t inMatrix) {
        ColumnFloat_t sumCols = arma::sum(inMatrix, 0).t();
        ColumnFloat_t weightedSumCols = arma::regspace<ColumnFloat_t>(1.0f, static_cast<float>(inMatrix.n_cols)) % sumCols;
        float c = arma::sum(sumCols) == 0.0f ? static_cast<float>(inMatrix.n_cols - 1) / 2.0f :
                  arma::sum(weightedSumCols) / arma::sum(sumCols) - 1.0f;

        ColumnFloat_t sumRows = arma::sum(inMatrix, 1);
        ColumnFloat_t weightedSumRows = arma::regspace<ColumnFloat_t>(1.0f, static_cast<float>(inMatrix.n_rows)) % sumRows;
        float r = arma::sum(sumRows) == 0.0f ? static_cast<float>(inMatrix.n_rows - 1) / 2.0f :
                  arma::sum(weightedSumRows) / arma::sum(sumRows) - 1.0f;

        return {r, c};
    }

    void computeLocalCorr(const CubeFloat_t & inData, MatrixFloat_t & outCorrMatrix)
    {
        // Get mean of each pixel along time axis (slices)
        const MatrixFloat_t mean = arma::mean(inData, 2);

        // Get stddev of each pixel
        MatrixFloat_t stddev(inData.n_rows, inData.n_cols);
        for (size_t colIndex = 0; colIndex < inData.n_cols; ++colIndex)
        {
            // Get a plane through the cube along a col - shape: (inData.n_rows, inData.n_slices)
            const MatrixFloat_t temp = inData(arma::span::all, arma::span(colIndex), arma::span::all);
            // Get stddev along time axis
            stddev.col(colIndex) = arma::stddev(temp, 1, 1);
        }

        // Set all zero values to infinity
        stddev.transform( [](float val) {return (val == 0) ? std::numeric_limits<float>::max() : val; } );

        // Process input data
        CubeFloat_t processedData = inData;

        // Create convolution filter for 8 neighbours
        // Perform 2D seperable convolution as 2x1D convolutions
        MatrixFloat_t filter = {{1, 1, 1}};
        cv::Mat rowFilter = armaToCvMat(filter);
        cv::Mat colFilter = armaToCvMat(filter, true);

        for (size_t slice = 0; slice < processedData.n_slices; ++slice)
        {
            processedData.slice(slice) -= mean;
            processedData.slice(slice) /= stddev;
            cv::Mat mat = armaToCvMat(processedData.slice(slice));
            cv::sepFilter2D(mat, mat, -1, rowFilter, colFilter, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            // Subtract each pixel's value from it's correlation result after performing the 2x1D convolution trick
            processedData.slice(slice) = (cvToArmaMat<float>(mat) - processedData.slice(slice)) % processedData.slice(slice);
        }

        cv::Mat maskMat = cv::Mat(static_cast<int>(processedData.n_rows), static_cast<int>(processedData.n_cols), CV_64FC1, 1);
        cv::sepFilter2D(maskMat, maskMat, -1, rowFilter, colFilter, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
        MatrixFloat_t mask = cvToArmaMat<float>(maskMat) - arma::ones<MatrixFloat_t>(processedData.n_rows, processedData.n_cols);

        outCorrMatrix = arma::mean(processedData, 2);
        outCorrMatrix /= mask;
    }
}
