#ifndef ISX_CNMFE_H
#define ISX_CNMFE_H

#include "isxCnmfeParams.h"

namespace isx
{
    class Cnmfe
    {
        public:

            /// Default constructor
            Cnmfe();

            /// Fully specified constructor
            Cnmfe(
                const DeconvolutionParams inDeconvParams,
                const InitializationParams inInitParams,
                const SpatialParams inSpatialParams,
                const int32_t maxNumNeurons,
                const float ringSizeFactor,
                const float mergeThresh,
                const size_t numIterations,
                const size_t numThreads,
                const bool outputFinalTraces = false);

            /// Fits the CNMFe model to the data and extracts spatiotemporal components
            void fit(const CubeFloat_t & inY);

            /// Returns the spatial components
            const CubeFloat_t & getSpatialComponents() const;

            /// Returns the temporal components
            const MatrixFloat_t & getTemporalComponents() const;

            /// Returns the raw temporal components
            const MatrixFloat_t & getRawTemporalComponents();

            /// Returns the background spatial components
            const arma::SpMat<float> & getSpatialBackground() const;

            /// Returns the background temporal components
            const MatrixFloat_t & getTemporalBackground() const;

            /// Sets the spatial components
            void setSpatialComponents(const CubeFloat_t & spatialComponents);

            /// Sets the temporal components
            void setTemporalComponents(const MatrixFloat_t & temporalComponents);

            /// Sets the raw temporal components
            void setRawTemporalComponents(const MatrixFloat_t & rawTemporalComponents);
            
            /// Sets the background spatial components
            void setSpatialBackground(const arma::SpMat<float> & spatialBackground);

            /// Sets the background temporal components
            void setTemporalBackground(const MatrixFloat_t & temporalBackground);

            /// Returns the total number of neurons extracted from the source movie
            size_t getNumNeurons();

            /// Returns the number of frames in the source movie
            size_t getNumFrames();

            /// Returns the number of pixels in the source movie
            std::pair<size_t, size_t> getNumPixels();

            /// Returns the deconvolution parameters
            const DeconvolutionParams & getDeconvParams();

            /// Returns the initialization parameters
            const InitializationParams & getInitParams();

            /// Returns the spatial parameters
            const SpatialParams & getSpatialParams();

            /// Returns the maximum number of neurons to detect during initialization
            int32_t getMaxNumNeurons();

            /// Returns the ratio of ring radius to neuron diameter used in background estimation
            float getRingSizeFactor();

            /// Returns the threshold for merging components
            float getMergeThreshold();

            /// Returns the number of iterations for initialization
            size_t getNumIterations();

            /// Returns the number of threads to use when parallelization is possible 
            size_t getNumThreads();

        private:

            /// Spatial footprints of neurons (d1 x d2 x K)
            CubeFloat_t m_A;

            /// Deconvolved temporal activity of neurons (K x T)
            MatrixFloat_t m_C;

            /// Raw temporal activity of neurons (K x T)
            MatrixFloat_t m_rawC;

            /// Background spatial components (d x d)
            arma::SpMat<float> m_spatialB;

            /// Background temporal components (d x T)
            MatrixFloat_t m_temporalB;

            /// Noise estimation per pixel (d1 x d2)
            MatrixFloat_t m_noise;

            /// Deconvolution parameters
            DeconvolutionParams m_deconvParams;

            /// Initialization parameters
            InitializationParams m_initParams;

            /// Spatial parameters
            SpatialParams m_spatialParams;

            /// Maximum number of neurons to detect (-1 stops when all pixels are below thresholds)
            int32_t m_maxNumNeurons;

            /// Ratio of ring radius to neuron diameter
            float m_ringSizeFactor;

            /// Threshold for merging components
            float m_mergeThresh;

            /// Number of iterations for initialization
            size_t m_numIterations;

            /// Number of threads to use when parallelization is possible
            size_t m_numThreads;

            /// Indicates whether to output final deconvolved traces (used in patch mode for merging components)
            bool m_outputFinalTraces;

    }; // class
}  // namespace isx

#endif //ISX_CNMFE_H
