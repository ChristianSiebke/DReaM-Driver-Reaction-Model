SET (SOURCES dynamics_longitudinal_basic.cpp
			 dynamics_longitudinal_basic_implementation.cpp)
SET (HEADERS dynamics_longitudinal_basic.h
			 dynamics_longitudinal_basic_implementation.h
			 dynamics_longitudinal_basic_global.h)

			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})

add_definitions(-DDYNAMICS_LONGITUDINAL_BASIC_LIBRARY)

OPENPASS_add_component(Dynamics_Longitudinal_Basic "")

target_link_libraries(Dynamics_Longitudinal_Basic Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})