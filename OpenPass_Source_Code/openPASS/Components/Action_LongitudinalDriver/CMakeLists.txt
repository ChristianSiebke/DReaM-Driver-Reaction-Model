SET (SOURCES action_longitudinalDriver.cpp
			 action_longitudinalDriverImplementation.cpp)
SET (HEADERS action_longitudinalDriver.h
			 action_longitudinalDriverGlobal.h
			 action_longitudinalDriverImplementation.h)

add_definitions(-DACTION_LONGITUDINAL_DRIVER_LIBRARY)

OPENPASS_add_component(Action_LongitudinalDriver "")
