

set(FOLDER "AgentStateRecorder")

set(COMPONENT_NAME AgentStateRecorder)

add_compile_definitions(AgentStateRecorder_EXPORTS)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT common
  
  HEADERS
    AgentStateRecorder.h
    ../Components/GazeMovement/RoadSegments/RoadSegmentInterface.h
  SOURCES
    AgentStateRecorder.cpp
  INCDIRS
    .
 
  LIBRARIES
    TUDresdenCommon
)
