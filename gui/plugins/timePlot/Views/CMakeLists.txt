# /*********************************************************************
# * Copyright (c) 2020 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

set(SOURCES
        ViewTimePlot.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/Plot.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/RowHistograms.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/CenteredTextItem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/PlotGraphicsItem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/Graph.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/PlotAxes.cpp
    )

set(HEADERS
        ViewTimePlot.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/Plot.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/RowHistograms.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/CenteredTextItem.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/PlotGraphicsItem.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/Graph.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/PlotAxes.h
    )

set(UI
        ViewTimePlot.ui
     )

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Widgets)
