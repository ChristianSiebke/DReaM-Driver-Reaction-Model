SET (SOURCES CollisionManipulator.cpp
			 ComponentStateChangeManipulator.cpp
			 LaneChangeManipulator.cpp
			 ManipulatorCommonBase.cpp
			 ManipulatorExport.cpp
			 RemoveAgentsManipulator.cpp)
SET (HEADERS CollisionManipulator.h
			 ComponentStateChangeManipulator.h
			 LaneChangeManipulator.h
			 ManipulatorCommonBase.h
			 ManipulatorExport.h
			 ManipulatorGlobal.h
			 RemoveAgentsManipulator.h)


add_definitions(-DMANIPULATOR_LIBRARY)
include_directories(../../Common ../../Interfaces)

OPENPASS_ADD_MODULE(Manipulator "")
