# /*********************************************************************
# * Copyright (c) 2019, 2020 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

FORMS += \
    $$PWD/ViewTimePlot.ui

HEADERS += \
    $$PWD/ViewTimePlot.h \
#    $$PWD/Graph.h \
#    $$PWD/Plot.h \
#    $$PWD/PlotGraphicsItem.h \
#    $$PWD/PlotAxes.h \
    ../Common_GUI/Plot.h \
    ../Common_GUI/PlotAxes.h \
    ../Common_GUI/PlotGraphicsItem.h \
    ../Common_GUI/Graph.h \
    ../Common_GUI/CenteredTextItem.h


SOURCES += \
    $$PWD/ViewTimePlot.cpp \
#    $$PWD/Graph.cpp \
#    $$PWD/Plot.cpp \
#    $$PWD/PlotGraphicsItem.cpp \
#    $$PWD/PlotAxes.cpp \
    ../Common_GUI/Plot.cpp \
    ../Common_GUI/PlotAxes.cpp \
    ../Common_GUI/PlotGraphicsItem.cpp \
    ../Common_GUI/Graph.cpp \
    ../Common_GUI/CenteredTextItem.cpp

