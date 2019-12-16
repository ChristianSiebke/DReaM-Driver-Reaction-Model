# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

#-----------------------------------------------------------------------------
# \file  openPASS.pro
# \brief This file includes all the subdirectories storing the files for
#        the QtCreator-project for the OpenPass Gui
#-----------------------------------------------------------------------------

TEMPLATE = subdirs
SUBDIRS = \
    openPASS/openPASS.pro \
    openPASS-Component/openPASS-Component.pro \
    openPASS-Project/openPASS-Project.pro \
    openPASS-System/openPASS-System.pro \
    openPASS-Window/openPASS-Window.pro \
    openPASS-TrafficSimulation/openPASS-TrafficSimulation.pro \
    openPASS-AgentConfiguration/openPASS-AgentConfiguration.pro
