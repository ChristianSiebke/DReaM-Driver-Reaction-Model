/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QDomDocument>
#include "common/parameter.h"

namespace openpass::parameter
{

ParameterSetLevel1 Import(const QDomElement& parameterElement, const QDomElement& profilesElement);

} //namespace openpass::parameter
