#ifndef ISX_CNMFE_NOISE_H
#define ISX_CNMFE_NOISE_H

#include "isxArmaUtils.h"

namespace isx
{
    /// Method used for averaging the noise
    enum class AveragingMethod_t
    {
        MEAN = 0,
        MEDIAN,
        LOGMEXP
    };

    /// Computes the average of the power spectral density (PSD) for each pixel
    ///
    /// \param inData    Cube of movie data (h x w x t)
    /// \param outPSD    Average of the PSD
    /// \param method    Method for averaging the noise
    void averagePSD(
        const CubeFloat_t & inData,
        MatrixFloat_t & outPSD,
        const AveragingMethod_t method = AveragingMethod_t::LOGMEXP);

    /// Computes the average of the power spectral density (PSD) for each pixel
    ///
    /// \param inData    Column of pixel values over time
    /// \param method    Method for averaging the noise
    /// \return          Average of the PSD
    float averagePSD(
        const ColumnFloat_t & inData,
        const AveragingMethod_t method = AveragingMethod_t::LOGMEXP);

    /// Estimates noise level for each pixel by averaging the power spectral density
    ///
    /// \param inData          Cube of movie data (h x w x t)
    /// \param outNoise        Noise level for each pixel
    /// \param noiseRange      Range of frequencies over which power spectrum is averaged
    /// \param noiseMethod     Method for averaging the noise
    /// \param maxSamplesFft   Maximum number of samples to use in FFT
    void getNoiseFft(
        const CubeFloat_t & inData,
        MatrixFloat_t & outNoise,
        const std::pair<float,float> noiseRange = {0.25f, 0.5f},
        const AveragingMethod_t noiseMethod = AveragingMethod_t::LOGMEXP,
        const uint32_t maxSamplesFft = 4096);

    /// Estimates noise level for given pixel by averaging the power spectral density using FFT
    ///
    /// \param inData          Column of pixel values over time
    /// \param noiseRange      Range of frequencies over which power spectrum is averaged
    /// \param noiseMethod     Method for averaging the noise
    /// \param maxSamplesFft   Maximum number of samples to use in FFT
    /// \return                Estimated noise level for input pixel
    float getNoiseFft(
        const ColumnFloat_t & inData,
        const std::pair<float,float> noiseRange = {0.25f, 0.5f},
        const AveragingMethod_t noiseMethod = AveragingMethod_t::LOGMEXP,
        const uint32_t maxSamplesFft = 4096);
}

#endif //ISX_CNMFE_NOISE_H
