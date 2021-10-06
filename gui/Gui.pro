################################################################################
# Copyright (c) 2020 ITK Engineering GmbH
#               2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

#-----------------------------------------------------------------------------
# \file  openPASS.pro
# \brief This file includes all the subdirectories storing the files for
#        the QtCreator-project for the OpenPass Gui
#-----------------------------------------------------------------------------

TEMPLATE = subdirs
SUBDIRS = \
    application/openPASS.pro \
    plugins/component/openPASS-Component.pro \
    plugins/window/openPASS-Window.pro \
    plugins/project/openPASS-Project.pro \
    plugins/statistics/openPASS-Statistics.pro \
    plugins/timePlot/openPASS-TimePlot.pro \
    plugins/pcmSimulation/openPASS-PCM.pro \
    plugins/pcmEvaluation/openPASS-PCM_Eval.pro \
    plugins/system/openPASS-System.pro \


#-----------------------------------------------------------------------------
# temporary excluded until compatiblity is established
    #plugins/agentConfiguration/openPASS-AgentConfiguration.pro \
    #plugins/trafficSimulation/openPASS-TrafficSimulation.pro \
