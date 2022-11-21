################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

find_package(Qt5 COMPONENTS Core Widgets Xml Sql REQUIRED)

set(SOURCES
    pcm/PCM_Data/pcm_agent.cpp     
    pcm/PCM_Data/pcm_line.cpp           
    pcm/PCM_Data/pcm_marks.cpp            
    pcm/PCM_Data/pcm_point.cpp           
    pcm/PCM_Data/pcm_trajectory.cpp  
    pcm/PCM_Data/pcm_globalData.cpp     
    pcm/PCM_Data/pcm_course.cpp      
    pcm/PCM_Data/pcm_lineContainer.cpp  
    pcm/PCM_Data/pcm_object.cpp           
    pcm/PCM_Data/pcm_pointContainer.cpp  
    pcm/PCM_Data/pcm_viewObject.cpp 
    pcm/PCM_Data/pcm_data.cpp    
    pcm/PCM_Data/pcm_initialValues.cpp  
    pcm/PCM_Data/pcm_lineSegment.cpp    
    pcm/PCM_Data/pcm_participantData.cpp  
    pcm/PCM_Data/pcm_simulationSet.cpp

    pcm/PCM_Importer/scenarioImporterPCM.cpp  
    pcm/PCM_Importer/sceneryImporterPCM.cpp  
    pcm/PCM_Importer/vehicleModelImporter.cpp

    DelegateDoubleView.cpp  
    Plot.cpp              
    WidgetView.cpp
    CenteredTextItem.cpp       
    Graph.cpp                 
    RowHistograms.cpp    
    DelegateTimeView.cpp            
    PlotAxes.cpp           
    DelegateComboBoxView.cpp     
    Histogram.cpp           
    TableModel.cpp 
    PlotGraphicsItem.cpp  
)

set(HEADERS
    pcm/DataStructuresBase/XmlBaseClass.h
    pcm/DataStructuresBase/XmlBaseTypes.h

    pcm/PCM_Data/pcm_definitions.h      
    pcm/PCM_Data/pcm_agent.h         
    pcm/PCM_Data/pcm_line.h             
    pcm/PCM_Data/pcm_marks.h              
    pcm/PCM_Data/pcm_point.h             
    pcm/PCM_Data/pcm_trajectory.h
    pcm/PCM_Data/pcm_course.cpp  
    pcm/PCM_Data/pcm_globalData.h       
    pcm/PCM_Data/pcm_course.h    
    pcm/PCM_Data/pcm_helper.h           
    pcm/PCM_Data/pcm_lineContainer.h    
    pcm/PCM_Data/pcm_object.h             
    pcm/PCM_Data/pcm_pointContainer.h    
    pcm/PCM_Data/pcm_viewObject.h
    pcm/PCM_Data/pcm_data.h      
    pcm/PCM_Data/pcm_initialValues.h    
    pcm/PCM_Data/pcm_lineSegment.h      
    pcm/PCM_Data/pcm_participantData.h    
    pcm/PCM_Data/pcm_simulationSet.h

    pcm/PCM_Importer/scenarioImporterPCM.h    
    pcm/PCM_Importer/sceneryImporterPCM.h    
    pcm/PCM_Importer/vehicleModelImporter.h

    GUI_Definitions.h              
    PlotGraphicsItem.h     
    DelegateDoubleView.h         
    Plot.h                
    WidgetView.h
    CenteredTextItem.h         
    Graph.h                  
    RowHistograms.h     
    DelegateTimeView.h       
    PlotAxes.h            
    DelegateComboBoxView.h    
    EditDataCommand.h       
    Histogram.h        
    TableModel.h
)

include_directories(../../sim/src/common)
include_directories(../../sim/src)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(pcm/DataStructuresBase)
include_directories(pcm/PCM_Data)
include_directories(pcm/PCM_Importer)

add_library(Gui_Common STATIC ${SOURCES} ${HEADERS})

target_link_libraries(Gui_Common PRIVATE
                      Qt5::Widgets
                      Qt5::Core
                      Qt5::Xml
                      Qt5::Sql
)