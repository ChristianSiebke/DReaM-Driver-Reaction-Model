/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <limits>
#include <variant>

#include "commonHelper.h"

namespace openpass::parameter {

struct NormalDistribution
{
    NormalDistribution() = default;
    NormalDistribution(const NormalDistribution&) = default;
    NormalDistribution(NormalDistribution&&) = default;
    NormalDistribution& operator=(const NormalDistribution&) = default;
    NormalDistribution& operator=(NormalDistribution&&) = default;

    NormalDistribution(double mean, double standardDeviation, double min, double max):
        mean{mean}, standardDeviation{standardDeviation}, min{min}, max{max} {}

    double mean{0.0};
    double standardDeviation{0.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

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
};

struct LogNormalDistribution
{
    LogNormalDistribution() = default;
    LogNormalDistribution(const LogNormalDistribution&) = default;
    LogNormalDistribution(LogNormalDistribution&&) = default;
    LogNormalDistribution& operator=(const LogNormalDistribution&) = default;
    LogNormalDistribution& operator=(LogNormalDistribution&&) = default;

    LogNormalDistribution(double mu, double sigma, double min, double max):
        mu{mu}, sigma{sigma}, min{min}, max{max} {}

    double mu{0.0};
    double sigma{0.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

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
};

struct UniformDistribution
{
    UniformDistribution() = default;
    UniformDistribution(const UniformDistribution&) = default;
    UniformDistribution(UniformDistribution&&) = default;
    UniformDistribution& operator=(const UniformDistribution&) = default;
    UniformDistribution& operator=(UniformDistribution&&) = default;

    UniformDistribution(double min, double max):
        min{min}, max{max} {}

    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

    bool operator==(const UniformDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }
};

struct ExponentialDistribution
{
    ExponentialDistribution() = default;
    ExponentialDistribution(const ExponentialDistribution&) = default;
    ExponentialDistribution(ExponentialDistribution&&) = default;
    ExponentialDistribution& operator=(const ExponentialDistribution&) = default;
    ExponentialDistribution& operator=(ExponentialDistribution&&) = default;

    ExponentialDistribution(double lambda, double min, double max):
        lambda{lambda}, min{min}, max{max} {}

    double lambda{1.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};

    bool operator==(const ExponentialDistribution& rhs) const
    {
        return this == &rhs || (
               CommonHelper::DoubleEquality(lambda, rhs.lambda) &&
               CommonHelper::DoubleEquality(min, rhs.min) &&
               CommonHelper::DoubleEquality(max, rhs.max));
    }
};

using StochasticDistribution = std::variant<NormalDistribution, LogNormalDistribution, UniformDistribution, ExponentialDistribution>;

} // openpass::parameter
