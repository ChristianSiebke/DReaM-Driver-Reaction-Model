#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

SET (SOURCES SystemPlugin.cpp
Models/SystemXMLSaveModel.cpp
Models/SystemComponentInputItemModel.cpp
Models/SystemXMLLoadModel.cpp
Models/SystemComponentInputMapModel.cpp
Models/SystemXMLBaseModel.cpp
Models/SystemComponentItemModel.cpp
Models/SystemModel.cpp
Models/SystemComponentManagerModel.cpp
Models/SystemMapModel.cpp
Models/SystemComponentMapModel.cpp
Models/SystemItemModel.cpp
Models/SystemComponentOutputItemModel.cpp
Models/SystemConnectionMapModel.cpp
Models/SystemComponentOutputMapModel.cpp
Models/SystemConnectionItemModel.cpp
Models/SystemComponentParameterItemModel.cpp
Models/SystemComponentScheduleModel.cpp
Models/SystemComponentParameterMapModel.cpp
Views/WidgetView.cpp
Views/DoubleSpinBoxView.cpp
Views/SystemView.cpp
Views/SpinBoxView.cpp
Views/SystemItemView.cpp
Views/SystemBaseView.cpp
Views/SystemComponentTitleView.cpp
Views/SystemComponentInputItemView.cpp
Views/SystemComponentScheduleView.cpp
Views/SystemComponentParameterValueStringView.cpp
Views/SystemComponentItemView.cpp
Views/SystemComponentParameterValueIntView.cpp
Views/SystemComponentParameterValueFloatView.cpp
Views/SystemComponentManagerView.cpp
Views/SystemComponentParameterValueDoubleView.cpp
Views/SystemComponentParameterValueBoolView.cpp
Views/SystemComponentParameterValueNormalDistributionView.cpp
Views/SystemComponentOutputItemView.cpp
Views/SystemComponentParameterItemView.cpp
Views/SystemTabView.cpp
)
SET (HEADERS SystemPlugin.h 
Models/SystemComponentParameterMapModel.h
Models/SystemComponentParameterItemModel.h
Models/SystemComponentScheduleModel.h
Models/SystemComponentOutputMapModel.h
Models/SystemConnectionItemModel.h
Models/SystemComponentOutputItemModel.h
Models/SystemConnectionMapModel.h
Models/SystemComponentMapModel.h
Models/SystemItemModel.h
Models/SystemComponentManagerModel.h
Models/SystemMapModel.h
Models/SystemComponentItemModel.h
Models/SystemModel.h
Models/SystemComponentInputMapModel.h
Models/SystemXMLBaseModel.h
Models/SystemComponentInputItemModel.h
Models/SystemXMLLoadModel.h
Models/SystemXMLSaveModel.h
Views/SystemComponentInputItemView.h
Views/SystemComponentOutputItemView.h
Views/SystemComponentParameterItemView.h
Views/SystemView.h
Views/DoubleSpinBoxView.h
Views/SystemComponentParameterValueBoolView.h
Views/SystemComponentManagerView.h
Views/SystemComponentParameterValueDoubleView.h
Views/SystemComponentParameterValueNormalDistributionView.h
Views/SystemItemView.h
Views/SystemComponentParameterValueFloatView.h
Views/SystemComponentItemView.h
Views/SystemComponentParameterValueIntView.h
Views/SpinBoxView.h
Views/SystemComponentParameterValueStringView.h
Views/WidgetView.h
Views/SystemComponentScheduleView.h
Views/SystemComponentTitleView.h
Views/SystemBaseView.h
Views/SystemTabView.h
Interfaces/openPASS-System/SystemItemInterface.h
Interfaces/openPASS-System/SystemComponentInputItemInterface.h
Interfaces/openPASS-System/SystemComponentInputMapInterface.h
Interfaces/openPASS-System/SystemComponentItemInterface.h
Interfaces/openPASS-System/SystemComponentManagerInterface.h
Interfaces/openPASS-System/SystemComponentMapInterface.h
Interfaces/openPASS-System/SystemComponentOutputItemInterface.h
Interfaces/openPASS-System/SystemComponentOutputMapInterface.h
Interfaces/openPASS-System/SystemComponentParameterItemInterface.h
Interfaces/openPASS-System/SystemComponentParameterMapInterface.h
Interfaces/openPASS-System/SystemComponentScheduleInterface.h
Interfaces/openPASS-System/SystemConnectionItemInterface.h
Interfaces/openPASS-System/SystemConnectionMapInterface.h
Interfaces/openPASS-System/SystemInterface.h
Interfaces/openPASS-System/SystemMapInterface.h
)

SET(UIS 
Views/SystemComponentItemView.ui
Views/SystemComponentManagerView.ui
Views/SystemComponentParameterItemView.ui
Views/SystemView.ui
Views/SystemComponentInputItemView.ui
Views/SystemComponentTitleView.ui
Views/SystemComponentScheduleView.ui
Views/SystemComponentOutputItemView.ui
Views/SystemTabView.ui
)


INCLUDE_DIRECTORIES(.)
add_definitions(-DSystem_LIBRARY)

OPENPASS_GUI_add_plugin(System "")

target_link_libraries(System Component Project)

