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
   DoubleSpinBoxView.cpp
      SpinBoxView.cpp
      SystemBaseView.cpp
      SystemComponentInputItemView.cpp
      SystemComponentItemView.cpp
      SystemComponentManagerView.cpp
      SystemComponentOutputItemView.cpp
      SystemComponentParameterItemView.cpp
      SystemComponentParameterValueBoolView.cpp
      SystemComponentParameterValueDoubleView.cpp
      SystemComponentParameterValueFloatView.cpp
      SystemComponentParameterValueIntView.cpp
      SystemComponentParameterValueNormalDistributionView.cpp
      SystemComponentParameterValueStringView.cpp
      SystemComponentScheduleView.cpp
      SystemComponentTitleView.cpp
      SystemItemView.cpp
      SystemView.cpp
      WidgetView.cpp
      SystemTabView.cpp
    )

set(HEADERS
   DoubleSpinBoxView.h
       SpinBoxView.h
       SystemBaseView.h
       SystemComponentInputItemView.h
       SystemComponentItemView.h
       SystemComponentManagerView.h
       SystemComponentOutputItemView.h
       SystemComponentParameterItemView.h
       SystemComponentParameterValueBoolView.h
       SystemComponentParameterValueDoubleView.h
       SystemComponentParameterValueFloatView.h
       SystemComponentParameterValueIntView.h
       SystemComponentParameterValueNormalDistributionView.h
       SystemComponentParameterValueStringView.h
       SystemComponentScheduleView.h
       SystemComponentTitleView.h
       SystemItemView.h
       SystemView.h
       WidgetView.h
       SystemTabView.h
    )

set(UI
       SystemComponentInputItemView.ui
       SystemComponentItemView.ui
       SystemComponentManagerView.ui
       SystemComponentOutputItemView.ui
       SystemComponentParameterItemView.ui
       SystemComponentScheduleView.ui
       SystemComponentTitleView.ui
       SystemView.ui
       SystemTabView.ui
     )

add_library(${PROJECT_NAME}_Views STATIC ${SOURCES} ${HEADERS} ${UI})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Widgets)
