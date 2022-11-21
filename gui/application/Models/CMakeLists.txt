################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

set(SOURCES
    PluginManagerModel.cpp
    ServiceManagerModel.cpp
)

set(HEADERS
    PluginManagerModel.h
    ServiceManagerModel.h
)

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})

