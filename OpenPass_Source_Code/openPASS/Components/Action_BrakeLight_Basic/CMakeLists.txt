SET (SOURCES action_brakelight_basic.cpp
			 action_brakelight_basic_implementation.cpp)
SET (HEADERS action_brakelight_basic.h
			 action_brakelight_basic_implementation.h
			 action_brakelight_basic_global.h)


add_definitions(-DACTION_BRAKELIGHT_BASIC_LIBRARY)

OPENPASS_add_component(Action_BrakeLight_Basic "")