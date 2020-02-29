SET (SOURCES AlgorithmAgentFollowingDriverModel.cpp
			 AlgorithmAgentFollowingDriverModelImplementation.cpp)
SET (HEADERS AlgorithmAgentFollowingDriverModel.h
			 AlgorithmAgentFollowingDriverModelGlobal.h
			 AlgorithmAgentFollowingDriverModelImplementation.h)
			 
include_directories(${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
add_definitions(-DALGORITHM_AGENTFOLLOWINGDRIVERMODEL_LIBRARY)

OPENPASS_add_component(AlgorithmAgentFollowingDriverModel "")
target_link_libraries(AlgorithmAgentFollowingDriverModel Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})
