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
    application/openPASS.pro \
    #plugins/agentConfiguration/openPASS-AgentConfiguration.pro \
    plugins/component/openPASS-Component.pro \
    plugins/project/openPASS-Project.pro \
    plugins/statistics/openPASS-Statistics.pro \
    plugins/system/openPASS-System.pro \
    plugins/timePlot/openPASS-TimePlot.pro \
    #plugins/trafficSimulation/openPASS-TrafficSimulation.pro \
    plugins/window/openPASS-Window.pro \
