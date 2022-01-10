################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# set headers
set(HEADERS "")

# build all Interface libraries
include(openPASS-AgentConfiguration/CMakeLists.txt)
include(../../project/Interfaces/openPASS-Project/CMakeLists.txt)

# create interface library
add_library(${PROJECT_NAME}_Interfaces STATIC ${HEADERS})
