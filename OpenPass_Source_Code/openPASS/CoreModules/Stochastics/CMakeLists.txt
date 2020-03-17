#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES stochastics.cpp
			 stochastics_implementation.cpp)
set(HEADERS stochastics.h
			 stochastics_implementation.h
			 stochastics_global.h)


add_compile_definitions(STOCHASTICS_LIBRARY)

OPENPASS_ADD_MODULE(Stochastics "")
