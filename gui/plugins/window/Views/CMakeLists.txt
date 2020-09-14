# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/WindowView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/LogoDelegate.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/WindowView.h
    ${CMAKE_CURRENT_LIST_DIR}/LogoDelegate.h
    )

set(UI
    ${CMAKE_CURRENT_LIST_DIR}/WindowView.ui
    )

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Widgets)
