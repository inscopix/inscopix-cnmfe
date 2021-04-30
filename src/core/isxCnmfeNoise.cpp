#include "isxCnmfeNoise.h"
#include <cmath>

namespace isx
{
    void averagePSD(
        const CubeFloat_t & inData,
        MatrixFloat_t & outPSD,
        const AveragingMethod_t method)
    {
        switch(method)
        {
            case AveragingMethod_t::MEAN:
            {
                outPSD = arma::sqrt(arma::mean(inData / 2, 2));
                break;
            }
            case AveragingMethod_t::MEDIAN:
            {
                const CubeFloat_t inDataTransformed(inData / 2);
                outPSD = MatrixFloat_t(inDataTransformed.n_rows, inDataTransformed.n_cols);
                for (size_t colIndex = 0; colIndex < inDataTransformed.n_cols; ++colIndex)
                {
                    // Get a plane through the cube along a row (shape: inData.n_cols x inData.n_slices)
                    const MatrixFloat_t temp = inDataTransformed(arma::span::all, arma::span(colIndex), arma::span::all);
                    // Get sqrt of median along time axis
                    outPSD.col(colIndex) = arma::sqrt(arma::median(temp, 1));
                }
                break;
            }
            case AveragingMethod_t::LOGMEXP:
            {
                const CubeFloat_t inDataLogTransformed(arma::log(inData/2 + 1e-10f));
                outPSD = MatrixFloat_t(inDataLogTransformed.n_rows, inDataLogTransformed.n_cols);
                for (size_t colIndex = 0; colIndex < inDataLogTransformed.n_cols; ++colIndex)
                {
                    // Get a plane through the cube along a row (shape: inData.n_cols x inData.n_slices)
                    const MatrixFloat_t temp = inDataLogTransformed(arma::span::all, arma::span(colIndex), arma::span::all);
                    // Get sqrt of e^x where x is the mean along time axis
                    outPSD.col(colIndex) = arma::sqrt(arma::exp(arma::mean(temp, 1)));
                }
                break;
            }
        }
    }

    float averagePSD(const ColumnFloat_t & inData, const AveragingMethod_t method)
    {
        switch(method)
        {
            case AveragingMethod_t::MEAN:
                return std::sqrt(arma::mean(inData / 2));
            case AveragingMethod_t::MEDIAN:
                return std::sqrt(arma::median(inData / 2));
            case AveragingMethod_t::LOGMEXP:
            default:
                return std::sqrt(std::exp(arma::mean(arma::log(inData/2 + 1e-10f))));
        }
    }

    void subsample(
        const CubeFloat_t & inData,
        CubeFloat_t & outData,
        const uint32_t maxSamples)
    {
        outData = arma::join_slices(
            inData.subcube(arma::span::all, arma::span::all,arma::span(1,maxSamples/3)),
            inData.subcube(arma::span::all, arma::span::all,arma::span(
                (uint32_t)(inData.n_slices/2 - maxSamples/6.0f),
                (uint32_t)(inData.n_slices/2 + maxSamples/6.0f - 1))));

        outData = arma::join_slices(outData,inData.subcube(arma::span::all, arma::span::all,
                arma::span(inData.n_slices - maxSamples/3 - 1, inData.n_slices - 1)));
    }

    void subsample(
        const ColumnFloat_t & inData,
        ColumnFloat_t & outData,
        const uint32_t maxSamples)
    {
        outData = arma::join_cols(inData(arma::span(1, maxSamples/3)),
            inData(arma::span((uint32_t)(inData.size()/2 - maxSamples/6.0f),
                (uint32_t)(inData.size()/2 + maxSamples/6.0f - 1))));

        outData = arma::join_cols(outData,inData(arma::span(inData.size() - maxSamples/3,
            inData.size() -1)));
    }

    void getNoiseFft(
        const CubeFloat_t & inData,
        MatrixFloat_t & outNoise,
        const std::pair<float,float> noiseRange,
        const AveragingMethod_t noiseMethod,
        const uint32_t maxSamplesFft)
    {
        CubeFloat_t data;
        if (inData.n_slices > maxSamplesFft)
        {
            subsample(inData, data, maxSamplesFft);
        }
        else if (inData.n_slices > 2048)
        {
            subsample(inData, data, 2048);
        }
        else if (inData.n_slices > 1024)
        {
            subsample(inData, data, 1024);
        }
        else
        {
            data = inData;
        }

        // list of indices within desired frequency range
        ColumnFloat_t ff = arma::regspace<ColumnFloat_t>(0, 1.0f/data.n_slices, 0.5f);
        arma::uvec ind = arma::find((ff > noiseRange.first) && (ff <= noiseRange.second));

        // compute fft on input cube
        arma::cx_fcube xdft(data.n_rows, data.n_cols, data.n_slices);
        for (size_t colIndex = 0; colIndex < data.n_cols; ++colIndex)
        {
            for (size_t rowIndex = 0; rowIndex < data.n_rows; ++rowIndex)
            {
                ColumnFloat_t vec = data.subcube(arma::span(rowIndex), arma::span(colIndex), arma::span::all);
                xdft(arma::span(rowIndex), arma::span(colIndex), arma::span::all) = arma::fft(vec);
            }
        }

        arma::cx_fcube psdx_cx(data.n_rows, data.n_cols, ind.size());
        for (size_t i = 0; i < ind.size(); ++i)
        {
            psdx_cx.slice(i) = xdft.subcube(arma::span::all, arma::span::all, arma::span(ind.at(i)));
        }

        // compute pixel noise by averaging psd
        CubeFloat_t psdx(2 * 1.0f/data.n_slices * arma::square(arma::abs(psdx_cx)));
        averagePSD(psdx, outNoise, noiseMethod);
    }

    float getNoiseFft(
        const ColumnFloat_t & inData,
        const std::pair<float,float> noiseRange,
        const AveragingMethod_t noiseMethod,
        const uint32_t maxSamplesFft)
    {
        ColumnFloat_t data;
        if (inData.size() > maxSamplesFft)
        {
            subsample(inData, data, maxSamplesFft);
        }
        else if (inData.size() > 2048)
        {
            subsample(inData, data, 2048);
        }
        else if (inData.size() > 1024)
        {
            subsample(inData, data, 1024);
        }
        else
        {
            data = inData;
        }

        ColumnFloat_t ff = arma::regspace<ColumnFloat_t>(0, 1.0f/data.size(), 0.5f);
        arma::uvec ind = arma::find((ff > noiseRange.first) && (ff <= noiseRange.second));

        arma::cx_fvec fftVec = arma::fft(data);
        fftVec = fftVec.elem(ind);

        ColumnFloat_t psdx(2 * 1.0f/data.size() * arma::square(arma::abs(fftVec)));

        return averagePSD(psdx, noiseMethod);
    }
}
