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
        ModelTimePlot.cpp
        ResultDirItemModel.cpp
        TableModelPlot.cpp
   )

set(HEADERS
        ModelTimePlot.h
        ResultDirItemModel.h
        TableModelPlot.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
