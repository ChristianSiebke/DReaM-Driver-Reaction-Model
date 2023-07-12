

set(FOLDER "Analytics")

set(COMPONENT_NAME AnalysisDataRecorder)

add_compile_definitions(AnalysisDataRecorder_EXPORTS)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT common
  
  HEADERS
    AnalysisDataRecorder.h
    #../Components/GazeMovement/RoadSegments/RoadSegmentInterface.h
  SOURCES
    AnalysisDataRecorder.cpp
  INCDIRS
    .
 
  LIBRARIES
    TUDresdenCommon
)
