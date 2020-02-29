SET (SOURCES CollisionDetector.cpp
			 ConditionalEventDetector.cpp
			 EventDetectorCommonBase.cpp
			 EventDetectorExport.cpp)
SET (HEADERS CollisionDetector.h
			 ConditionalEventDetector.h
			 EventDetectorCommonBase.h
			 EventDetectorExport.h
			 EventDetectorGlobal.h
			 Conditions/ConditionCommonBase.h
			 Conditions/ReachPositionCondition.h
			 Conditions/SimulationTimeCondition.h)


add_definitions(-DEVENT_DETECTOR_LIBRARY)
include_directories(../../Common ../../Interfaces)

OPENPASS_ADD_MODULE(EventDetector "")