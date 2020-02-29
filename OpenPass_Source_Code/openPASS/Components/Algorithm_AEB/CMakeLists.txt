SET (SOURCES algorithm_autonomousEmergencyBraking.cpp
			 algorithm_autonomousEmergencyBrakingImplementation.cpp
			 boundingBoxCalculation.cpp)
SET (HEADERS algorithm_autonomousEmergencyBraking.h
			 algorithm_autonomousEmergencyBrakingGlobal.h
			 algorithm_autonomousEmergencyBrakingImplementation.h
			 boundingBoxCalculation.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})



add_definitions(-DALGORITHM_AEB_LIBRARY)

OPENPASS_add_component(Algorithm_AEB "")

target_link_libraries(Algorithm_AEB Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})