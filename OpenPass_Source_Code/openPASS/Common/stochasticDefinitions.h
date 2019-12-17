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

namespace openpass::parameter {

struct NormalDistribution
{
    NormalDistribution() {}
    NormalDistribution(double mean,
                       double standardDeviation,
                       double min,
                       double max):
        mean{mean},
        standardDeviation{standardDeviation},
        min{min},
        max{max}
    {}

    double mean{0.0};
    double standardDeviation{0.0};
    double min{std::numeric_limits<double>::lowest()};
    double max{std::numeric_limits<double>::max()};
};

} // openpass::parameter
