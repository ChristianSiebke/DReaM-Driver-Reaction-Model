SET (SOURCES algorithm_lateral.cpp
			 algorithm_lateralImplementation.cpp)
SET (HEADERS algorithm_lateral.h
			 algorithm_lateralGlobal.h
			 algorithm_lateralImplementation.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
add_definitions(-DALGORITHM_LATERAL_LIBRARY)

OPENPASS_add_component(Algorithm_Lateral "")

target_link_libraries(Algorithm_Lateral Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})