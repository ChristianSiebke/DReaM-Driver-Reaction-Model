#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

set(SOURCES ComponentPlugin.cpp 
Models/ComponentInputItemModel.cpp
Models/ComponentXMLSaveModel.cpp
Models/ComponentInputMapModel.cpp
Models/ComponentXMLLoadModel.cpp
Models/ComponentItemModel.cpp
Models/ComponentXMLBaseModel.cpp
Models/ComponentMapModel.cpp
Models/ComponentTypedefMapModel.cpp
Models/ComponentModel.cpp
Models/ComponentTypedefItemModel.cpp
Models/ComponentOutputItemModel.cpp
Models/ComponentScheduleModel.cpp
Models/ComponentOutputMapModel.cpp
Models/ComponentParameterMapModel.cpp
Models/ComponentParameterItemModel.cpp 
)
set(HEADERS ComponentPlugin.h 
Models/ComponentParameterItemModel.h
Models/ComponentOutputMapModel.h
Models/ComponentParameterMapModel.h
Models/ComponentOutputItemModel.h
Models/ComponentScheduleModel.h
Models/ComponentModel.h
Models/ComponentTypedefItemModel.h
Models/ComponentMapModel.h
Models/ComponentTypedefMapModel.h
Models/ComponentItemModel.h
Models/ComponentXMLBaseModel.h
Models/ComponentInputMapModel.h
Models/ComponentXMLLoadModel.h
Models/ComponentInputItemModel.h
Models/ComponentXMLSaveModel.h
Interfaces/openPASS-Component/ComponentInputItemInterface.h
Interfaces/openPASS-Component/ComponentInputMapInterface.h
Interfaces/openPASS-Component/ComponentInterface.h
Interfaces/openPASS-Component/ComponentItemInterface.h
Interfaces/openPASS-Component/ComponentMapInterface.h
Interfaces/openPASS-Component/ComponentOutputItemInterface.h
Interfaces/openPASS-Component/ComponentOutputMapInterface.h
Interfaces/openPASS-Component/ComponentParameterItemInterface.h
Interfaces/openPASS-Component/ComponentParameterMapInterface.h
Interfaces/openPASS-Component/ComponentScheduleInterface.h
Interfaces/openPASS-Component/ComponentTypedefItemInterface.h
Interfaces/openPASS-Component/ComponentTypedefMapInterface.h
)


include_directories(.)

add_compile_definitions(COMPONENT_LIBRARY)

openpass_gui_add_plugin(Component "")
target_link_libraries(Component openPASSInterfaces)
