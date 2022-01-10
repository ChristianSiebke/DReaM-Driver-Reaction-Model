/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <limits>
#include <variant>

#include "commonHelper.h"

namespace openpass::parameter {

class NormalDistribution
{
public:
    NormalDistribution() = default;
    NormalDistribution(const NormalDistribution&) = default;
    NormalDistribution(NormalDistribution&&) = default;
    NormalDistribution& operator=(const NormalDistribution&) = default;
    NormalDistribution& operator=(NormalDistribution&&) = default;

    NormalDistribution(double mean, double standardDeviation, double min, double max):
        mean{mean}, standardDeviation{standardDeviation}, min{min}, max{max} {}

private:
    double mean{0.0};
    double standardDeviation{0.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

public:
    bool operator==(const NormalDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(mean,rhs.mean) &&
               CommonHelper::DoubleEquality(standardDeviation, rhs.standardDeviation) &&
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }

    bool operator!=(const NormalDistribution& rhs) const
    {
        return !operator==(rhs);
    }

    double GetMin() const
    {
        return min;
    }

    double GetMax() const
    {
        return max;
    }

    double GetMean() const
    {
        return mean;
    }

    double GetStandardDeviation() const
    {
        return standardDeviation;
    }

    double GetMu() const
    {
        return mean;
    }

    double GetSigma() const
    {
        return standardDeviation;
    }

    void SetMin(double min)
    {
        this->min = min;
    }

    void SetMax(double max)
    {
        this->max = max;
    }

    void SetMean(double mean)
    {
        this->mean = mean;
    }

    void SetStandardDeviation(double standardDeviation)
    {
        this->standardDeviation = standardDeviation;
    }

    void SetMu(double mu)
    {
        mean = mu;
    }

    void SetSigma(double sigma)
    {
        standardDeviation = sigma;
    }
};

class LogNormalDistribution
{
public:
    LogNormalDistribution() = default;
    LogNormalDistribution(const LogNormalDistribution&) = default;
    LogNormalDistribution(LogNormalDistribution&&) = default;
    LogNormalDistribution& operator=(const LogNormalDistribution&) = default;
    LogNormalDistribution& operator=(LogNormalDistribution&&) = default;

    LogNormalDistribution(double mu, double sigma, double min, double max):
        mu{mu}, sigma{sigma}, min{min}, max{max}
    {
        mean = exp(mu + 0.5 * sigma * sigma);
        standardDeviation = mean * sqrt(exp(sigma * sigma) - 1);
    }

    static LogNormalDistribution CreateWithMeanSd(double mean, double standardDeviation, double min, double max)
    {
        double s2 = log(pow(standardDeviation/mean, 2)+1);
        double mu = log(mean)-s2/2;
        double sigma = sqrt(s2);
        return LogNormalDistribution(mu, sigma, min, max);
    }

private:
    double mu{0.0};
    double sigma{0.0};
    double mean{0.0};
    double standardDeviation{0.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

public:
    bool operator==(const LogNormalDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(mu, rhs.mu) &&
               CommonHelper::DoubleEquality(sigma, rhs.sigma) &&
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }

    bool operator!=(const LogNormalDistribution& rhs) const
    {
        return !operator==(rhs);
    }

    double GetMin() const
    {
        return min;
    }

    double GetMax() const
    {
        return max;
    }

    double GetMean() const
    {
        return mean;
    }

    double GetStandardDeviation() const
    {
        return standardDeviation;
    }

    double GetMu() const
    {
        return mu;
    }

    double GetSigma() const
    {
        return sigma;
    }

    void SetMin(double min)
    {
        this->min = min;
    }

    void SetMax(double max)
    {
        this->max = max;
    }

    void SetMean(double mean)
    {
        this->mean = mean;

        double s2 = log(pow(standardDeviation/mean, 2)+1);
        mu = log(mean)-s2/2;
        sigma = sqrt(s2);
    }

    void SetStandardDeviation(double standardDeviation)
    {
        this->standardDeviation = standardDeviation;

        double s2 = log(pow(standardDeviation/mean, 2)+1);
        mu = log(mean)-s2/2;
        sigma = sqrt(s2);
    }

    void SetMu(double mu)
    {
        this->mu = mu;

        mean = exp(mu + 0.5 * sigma * sigma);
        standardDeviation = mean * sqrt(exp(sigma * sigma) - 1);
    }

    void SetSigma(double sigma)
    {
        this->sigma = sigma;

        mean = exp(mu + 0.5 * sigma * sigma);
        standardDeviation = mean * sqrt(exp(sigma * sigma) - 1);
    }
};

class UniformDistribution
{
public:
    UniformDistribution() = default;
    UniformDistribution(const UniformDistribution&) = default;
    UniformDistribution(UniformDistribution&&) = default;
    UniformDistribution& operator=(const UniformDistribution&) = default;
    UniformDistribution& operator=(UniformDistribution&&) = default;

    UniformDistribution(double min, double max):
        min{min}, max{max} {}

private:
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

public:
    bool operator==(const UniformDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }

    double GetMin() const
    {
        return min;
    }

    double GetMax() const
    {
        return max;
    }

    void SetMin(double min)
    {
        this->min = min;
    }

    void SetMax(double max)
    {
        this->max = max;
    }
};

class ExponentialDistribution
{
public:
    ExponentialDistribution() = default;
    ExponentialDistribution(const ExponentialDistribution&) = default;
    ExponentialDistribution(ExponentialDistribution&&) = default;
    ExponentialDistribution& operator=(const ExponentialDistribution&) = default;
    ExponentialDistribution& operator=(ExponentialDistribution&&) = default;

    ExponentialDistribution(double lambda, double min, double max):
        lambda{lambda}, min{min}, max{max} {}

private:
    double lambda{1.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

public:
    bool operator==(const ExponentialDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(lambda, rhs.lambda) &&
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }

    double GetMin() const
    {
        return min;
    }

    double GetMax() const
    {
        return max;
    }

    double GetLambda() const
    {
        return lambda;
    }

    double GetMean() const
    {
        return 1 / lambda;
    }

    double GetStandardDeviation() const
    {
        return 1 / lambda;
    }

    void SetMin(double min)
    {
        this->min = min;
    }

    void SetMax(double max)
    {
        this->max = max;
    }

    void SetLambda(double lambda)
    {
        this->lambda = lambda;
    }

    void SetMean(double mean)
    {
        lambda = 1 /mean;
    }
};

class GammaDistribution
{
public:
    GammaDistribution() = default;
    GammaDistribution(const GammaDistribution&) = default;
    GammaDistribution(GammaDistribution&&) = default;
    GammaDistribution& operator=(const GammaDistribution&) = default;
    GammaDistribution& operator=(GammaDistribution&&) = default;

    GammaDistribution(double shape, double scale, double min, double max):
        shape{shape}, scale{scale}, min{min}, max{max}
    {
        mean = shape * scale;
        standardDeviation = sqrt(shape) * scale;
    }

    static GammaDistribution CreateWithMeanSd(double mean, double standardDeviation, double min, double max)
    {
        double shape = mean * mean / standardDeviation / standardDeviation;
        double scale = standardDeviation * standardDeviation / mean;
        return GammaDistribution(shape, scale, min, max);
    }

private:
    double shape{0.0};
    double scale{0.0};
    double mean{0.0};
    double standardDeviation{0.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

public:
    bool operator==(const GammaDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(shape, rhs.shape) &&
               CommonHelper::DoubleEquality(scale, rhs.scale) &&
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }

    bool operator!=(const GammaDistribution& rhs) const
    {
        return !operator==(rhs);
    }

    double GetMin() const
    {
        return min;
    }

    double GetMax() const
    {
        return max;
    }

    double GetMean() const
    {
        return mean;
    }

    double GetStandardDeviation() const
    {
        return standardDeviation;
    }

    double GetShape() const
    {
        return shape;
    }

    double GetScale() const
    {
        return scale;
    }

    void SetMin(double min)
    {
        this->min = min;
    }

    void SetMax(double max)
    {
        this->max = max;
    }

    void SetMean(double mean)
    {
        this->mean = mean;

        shape = mean * mean / standardDeviation / standardDeviation;
        scale = standardDeviation * standardDeviation / mean;
    }

    void SetStandardDeviation(double standardDeviation)
    {
        this->standardDeviation = standardDeviation;

        shape = mean * mean / standardDeviation / standardDeviation;
        scale = standardDeviation * standardDeviation / mean;
    }

    void SetShape(double shape)
    {
        this->shape = shape;

        mean = shape * scale;
        standardDeviation = sqrt(shape) * scale;
    }

    void SetScale(double scale)
    {
        this->scale = scale;

        mean = shape * scale;
        standardDeviation = sqrt(shape) * scale;
    }
};

using StochasticDistribution = std::variant<NormalDistribution, LogNormalDistribution, UniformDistribution, ExponentialDistribution, GammaDistribution>;

} // openpass::parameter
