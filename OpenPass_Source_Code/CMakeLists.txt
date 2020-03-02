################################################################################
# Copyright (c) 2020 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

if(WITH_SIMCORE)
  add_subdirectory(openPASS)
endif()

if(WITH_GUI)
  add_subdirectory(openPASS/Common)
  add_subdirectory(openPASS/CoreFramework/CoreShare)
  add_subdirectory(openPASS_GUI)
endif()
