################################################################################
# Copyright (c) 2019-2020 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

FORMS += \
    $$PWD/ViewTimePlot.ui

HEADERS += \
    $$PWD/ViewTimePlot.h \
#    $$PWD/Graph.h \
#    $$PWD/Plot.h \
#    $$PWD/PlotGraphicsItem.h \
#    $$PWD/PlotAxes.h \
    ../../common/Plot.h \
    ../../common/PlotAxes.h \
    ../../common/PlotGraphicsItem.h \
    ../../common/Graph.h \
    ../../common/CenteredTextItem.h


SOURCES += \
    $$PWD/ViewTimePlot.cpp \
#    $$PWD/Graph.cpp \
#    $$PWD/Plot.cpp \
#    $$PWD/PlotGraphicsItem.cpp \
#    $$PWD/PlotAxes.cpp \
    ../../common/Plot.cpp \
    ../../common/PlotAxes.cpp \
    ../../common/PlotGraphicsItem.cpp \
    ../../common/Graph.cpp \
    ../../common/CenteredTextItem.cpp

