#include "isxOasis.h"

namespace isx
{
    void Oasis::solveFoopsi(
        const ColumnFloat_t &inY,
        float &outB,
        float &outCa1,
        ColumnFloat_t &outC,
        ColumnFloat_t &outS)
    {
        fit(inY);
        outS = m_C;
        outS(0) = 0;
        outS(arma::span(1, m_T - 1)) -= m_gamma * m_C(arma::span(0, m_T - 2));

        // Remove initial calcium to align with other foopsi methods
        // Added back in constrainedFoopsiParallel
        outCa1 = m_C(0);
        outC = m_C - outCa1 * m_filter(arma::span(0, m_T - 1));
        outB = m_b;
    }

    void Oasis::fit(const ColumnFloat_t &inY)
    {
        initialize(inY);
        run(inY);
        updateHyperParameters(inY);

        size_t count = 0;
        ColumnFloat_t error = inY - m_C - m_b;
        float RSS = arma::dot(error, error);
        while (fabs(RSS - m_errorThreshold) > m_errorThreshold * 1e-4 && arma::sum(m_C) > 1e-9 && count < m_maxIterations)
        {
            updatePoolValues(error, RSS);
            run();
            updateHyperParameters(inY);
            error = inY - m_C - m_b;
            RSS = arma::dot(error, error);
            count += 1;
        }
    }

    void Oasis::initialize(const ColumnFloat_t &inY)
    {
        m_T = inY.n_elem;
        m_C = ColumnFloat_t(m_T);

        float tmp = 1.0f;
        m_filter = ColumnFloat_t(2 * m_T);
        for (size_t power = 0; power < 2 * m_T; power++)
        {
            m_filter(power) = tmp;
            tmp *= m_gamma;
        }

        m_b = nthPercentile(inY, 0.15f);
        m_errorThreshold = m_noise * m_noise * m_T;
    }

    void Oasis::run(const ColumnFloat_t &inY)
    {
        std::list<Pool>::iterator curr, prev;

        // Create pool for each value in Y
        m_pools.push_back(Pool(inY(0) - m_b, 1, 0, 1));
        for (size_t t = 1; t < m_T; t++)
        {
            m_pools.push_back(Pool(inY(t) - m_b, 1, t, 1));
            prev = curr = --m_pools.end(); // last element
            --prev;
            resolveViolations(prev, curr);
        }

        constructSolution();
    }

    void Oasis::run()
    {
        std::list<Pool>::iterator curr, prev;
        prev = curr = m_pools.begin();
        curr++;
        while (curr != m_pools.end())
        {
            resolveViolations(prev, curr);
            prev = curr++;
        }

        constructSolution();
    }

    void Oasis::resolveViolations(
        std::list<Pool>::iterator &prev,
        std::list<Pool>::iterator &curr)
    {
        while (curr != m_pools.begin() && // Backtrack until violations of calcium dynamics are resolved
               (*prev).m_v / (*prev).m_w * m_filter((*prev).m_l) > (*curr).m_v / (*curr).m_w)
        {
            // Merge pools
            (*prev).m_v += (*curr).m_v * m_filter((*prev).m_l);
            (*prev).m_w += (*curr).m_w * m_filter(2 * (*prev).m_l);
            (*prev).m_l += (*curr).m_l;

            curr = m_pools.erase(curr);
            --curr;
            --prev;
        }
    }

    void Oasis::constructSolution()
    {
        float tmp;
        for (const auto &pool : m_pools)
        {
            tmp = fmax(pool.m_v, 0.0f) / pool.m_w;
            for (size_t k = 0; k < pool.m_l; k++)
            {
                m_C(k + pool.m_t) = tmp;
                tmp *= m_gamma;
            }
        }
    }

    void Oasis::updatePoolValues(const ColumnFloat_t &error, const float RSS)
    {
        // Calculate total shift due to contribution of lambda and baseline
        ColumnFloat_t shift(m_T);
        std::list<Pool>::iterator lastIt = --m_pools.end();
        float tmp;
        for (std::list<Pool>::iterator it = m_pools.begin(); it != m_pools.end(); ++it)
        {
            if (it == lastIt)
            {
                tmp = 1.0f / (*it).m_w;
            }
            else
            {
                tmp = (1.0f - m_filter((*it).m_l)) / (*it).m_w;
            }
            for (size_t j = 0; j < (*it).m_l; j++)
            {
                shift((*it).m_t + j) = tmp;
                tmp *= m_gamma;
            }
        }
        tmp = 0.0f;
        for (const auto &pool : m_pools)
        {
            tmp += (1.0f - m_filter(pool.m_l)) * (1.0f - m_filter(pool.m_l)) / pool.m_w;
        }
        shift -= 1.0f / m_T / (1.0f - m_gamma) * tmp;
        float alpha = arma::dot(shift, shift);
        float beta = arma::dot(error, shift);
        float epsilon = RSS - m_errorThreshold;
        float deltaPhi;
        if (beta * beta - alpha * epsilon > 0.0f)
        {
            deltaPhi = (-beta + sqrt(beta * beta - alpha * epsilon)) / alpha;
        }
        else
        {
            deltaPhi = -beta / alpha;
        }

        m_b += deltaPhi * (1.0f - m_gamma);
        // Perform shift on pools
        for (std::list<Pool>::iterator it = m_pools.begin(); it != m_pools.end(); ++it)
        {
            (*it).m_v -= deltaPhi * (1.0f - m_filter((*it).m_l));
        }
    }

    void Oasis::updateHyperParameters(const ColumnFloat_t &inY)
    {
        float deltaB = arma::mean(inY - m_C) - m_b;
        m_b += deltaB;
        float deltaLambda = -deltaB / (1.0f - m_gamma);
        std::list<Pool>::iterator lastIt = --m_pools.end();
        (*lastIt).m_v -= deltaLambda * m_filter((*lastIt).m_l);
        m_C(arma::span((*lastIt).m_t, (*lastIt).m_t + (*lastIt).m_l - 1)) =
            fmax((*lastIt).m_v, 0.0f) / (*lastIt).m_w * m_filter(arma::span(0, (*lastIt).m_l - 1));
    }

} // namespace isx
