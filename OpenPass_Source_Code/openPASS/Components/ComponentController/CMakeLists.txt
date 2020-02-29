SET (SOURCES componentController.cpp
			 componentControllerImplementation.cpp
			 componentStateInformation.cpp
			 condition.cpp
			 stateManager.cpp)
SET (HEADERS componentController.h
			 componentControllerCommon.h
			 componentControllerGlobal.h
			 componentControllerImplementation.h
			 componentStateInformation.h
			 condition.h
			 stateManager.h)

add_definitions(-DCOMPONENT_CONTROLLER_LIBRARY)

OPENPASS_add_component(ComponentController "")
