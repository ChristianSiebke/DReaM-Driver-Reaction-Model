/********************************************************************************
 * Copyright (c) 2017 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/log.h"

std::map<int, std::unique_ptr<std::ofstream>> LogOutputPolicy::logStreamMap;
