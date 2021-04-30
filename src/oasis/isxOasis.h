#ifndef ISX_CNMFE_OASIS_H
#define ISX_CNMFE_OASIS_H

#include "isxArmaUtils.h"
#include <list>

namespace isx
{
    /// Collection of adjacent variables in an observed response
    /// Enables efficient calculation of sums in oasis
    struct Pool
    {
        float m_v;  ///< value
        float m_w;  ///< weight
        size_t m_t; ///< time
        size_t m_l; ///< pool length

        /// Default constructor
        Pool()
        {
        }

        /// Fully specified constructor
        Pool(
            float v,
            float w,
            size_t t,
            size_t l)
            : m_v(v), m_w(w), m_t(t), m_l(l)
        {
        }
    };

    /// Class implementing the OASIS model
    ///
    class Oasis
    {
    public:
        /// Constructor
        ///
        Oasis(
            const float inGamma,
            const float inNoise,
            const size_t inMaxIterations = 5)
            : m_gamma(inGamma), m_noise(inNoise), m_maxIterations(inMaxIterations) {}

        /// Denoise and deconvolve the calcium concentration dynamics and neural spike activity
        /// from a raw fluorescence trace using the OASIS algorithm
        /// For an autoregressive model of order p = 1 (i.e., AR1)
        /// Constructs outputs to align with outputs of other foopsi methods
        ///
        /// \param inY                Raw fluorescence trace
        /// \param outC               Denoised calcium trace
        /// \param outB               Final estimate of baseline
        /// \param outCa1             Initial calcium value (aligns with outputs of other foopsi methods)
        /// \param outS               Denoised and deconvolved neural spikes
        void solveFoopsi(
            const ColumnFloat_t &inY,
            float &outB,
            float &outCa1,
            ColumnFloat_t &outC,
            ColumnFloat_t &outS);

    private:
        const float m_gamma;           ///< Calcium dynamics decay constant
        const float m_noise;           ///< Standard deviation of estimated noise
        const size_t m_maxIterations;  ///< Maximum number of iteration to allow hyper-parameters to converge
        float m_errorThreshold;        ///< Hard constraint on the minimum acceptable error in the final solution generated
        float m_b;                     ///< Offset baseline
        size_t m_T;                    ///< Size of input temporal trace
        ColumnFloat_t m_C;             ///< Constructed solution of denoised calcium concentration dynamics
        ColumnFloat_t m_filter;        ///< Precomputed calcium kernel which appears often throughout calculations
        std::list<Pool> m_pools;       ///< Groups of values which satisfy constraints of calcium dynamics (i.e., grouping of spikes)

        /// Fits the model to the raw flurescence trace observed
        ///
        void fit(const ColumnFloat_t &inY);

        /// Initializes model variables
        ///
        void initialize(const ColumnFloat_t &inY);

        /// Run the OASIS algorithm
        /// Constructs an initial solution based on an observed response
        ///
        void run(const ColumnFloat_t &inY);

        /// Run the OASIS algorithm
        /// Updates warm-started solution to ensure there are no violations of the calcium dynamics
        /// Called after updating hyper-parameters
        ///
        void run();

        /// Resolve violations of the calcium dynamics constraint
        ///
        void resolveViolations(
            std::list<Pool>::iterator &prev,
            std::list<Pool>::iterator &curr);

        /// Construct solution for calcium concentration dynamics C
        ///
        void constructSolution();

        /// Update pools to minimize overall error in constructed solution
        ///
        void updatePoolValues(const ColumnFloat_t &error, const float RSS);

        /// Update hyper parameters to minimize overall error in constructed solution
        ///
        void updateHyperParameters(const ColumnFloat_t &inY);
    };

} // namespace isx

#endif // ISX_OASIS_H
