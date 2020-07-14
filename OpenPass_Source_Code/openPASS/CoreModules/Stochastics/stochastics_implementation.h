/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup Stochastics
* @{
* \brief implementation of StochasticsInterface
*
* \details The Stochastics module implements a StochasticsInterface which is used
* by all agents to create a random behavior.
*
* \section Stochastics_Inputs Inputs
* name | meaning
* -----|---------
* callbacks | CallbackInterface to send log information to the framework
*
* \section Stochastics_Outputs Outputs
 name | meaning
* -----|---------
* *StochasticsInterface | Provides a reference to an implementation of a StochasticsInterface to the framework.
*
* @see StochasticsInterface
*
* @} */

#pragma once

/// \brief prevents any long double functions from being defined or used with Boost.Math
/// this will reduce precision slightly, but will typically improve performance on 64-bit hardware
/// \remark make valgrind working if opimization is set by compiler flags -O2, -O3, or -Og
/// \sa     https://svn.boost.org/trac10/ticket/10005
#define BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
#define BOOST_MATH_PROMOTE_DOUBLE_POLICY false

#include <functional>
#include <random>
#include <vector>

#include "boost/math/distributions/lognormal.hpp"

#include "Interfaces/callbackInterface.h"
#include "Interfaces/stochasticsInterface.h"

using namespace boost::math;
using BinomialDist = std::binomial_distribution<>;

/*!
 * \brief implementation of StochasticsInterface
 *
 * The Stochastics module implements a StochasticsInterface which is used
 * by all agents to create a random behavior.
 *
 * \ingroup Stochastics
 */
class StochasticsImplementation : public StochasticsInterface
{
public:
    StochasticsImplementation(const CallbackInterface *callbacks);
    StochasticsImplementation(const StochasticsImplementation&) = delete;
    StochasticsImplementation(StochasticsImplementation&&) = delete;
    StochasticsImplementation& operator=(const StochasticsImplementation&) = delete;
    StochasticsImplementation& operator=(StochasticsImplementation&&) = delete;
    virtual ~StochasticsImplementation() override = default;

    double GetUniformDistributed(double a, double b) override;

    double GetNormalDistributed(double mean, double stdDeviation) override;

    int GetBinomialDistributed(int upperRangeNum, double probSuccess) override;

    double GetExponentialDistributed(double lambda) override;

    double GetGammaDistributed(double mean, double stdDeviation) override;

    double GetGammaDistributedShapeScale(double shape, double scale) override;

    double GetLogNormalDistributed(double mean, double stdDeviation) override;

    double GetLogNormalDistributedMuSigma(double mu, double sigma) override;

    double GetSpecialDistributed(std::string distributionName, std::vector<double> args) override;

    double GetRandomCdfLogNormalDistributed(double mean, double stdDeviation) override;

    double GetPercentileLogNormalDistributed(double mean, double stdDeviation, double probability) override;

    std::uint32_t GetRandomSeed() const override;

    void ReInit() override;

    void InitGenerator(std::uint32_t seed) override;

protected:
    /*! Provides callback to LOG() macro
    *
    * @param[in]     logLevel    Importance of log
    * @param[in]     file        Name of file where log is called
    * @param[in]     line        Line within file where log is called
    * @param[in]     message     Message to log
    */
    void Log(CbkLogLevel logLevel,
             const char *file,
             int line,
             const std::string &message)
    {
        if(callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

private:
    std::uint32_t randomSeed = 0;

    std::mt19937 baseGenerator;
    std::uniform_real_distribution<double> uniformDistribution;
    BinomialDist binomialDistribution;
    std::normal_distribution<double> normalDistribution;
    std::exponential_distribution<double> exponentialDistribution;

    const CallbackInterface *callbacks;
};
