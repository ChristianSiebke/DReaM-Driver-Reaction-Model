/*********************************************************************
* Copyright (c) 2017 - 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/callbackInterface.h"

class FakeCallback : public CallbackInterface
{
public:
	MOCK_CONST_METHOD4(Log, void(CbkLogLevel logLevel, const char *file, int line, const std::string &message));
};


