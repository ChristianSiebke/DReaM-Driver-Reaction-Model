# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

# define headers variable
set(HEADERS "")

# build all Interface libraries
include(openPASS-Window/CMakeLists.txt)
include(../../openPASS/Interfaces/openPASS/CMakeLists.txt)

# bundle all libraries to one interface library
add_library(${PROJECT_NAME}_Interfaces STATIC ${HEADERS})
