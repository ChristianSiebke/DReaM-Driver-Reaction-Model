SET (SOURCES action_secondaryDriverTasks.cpp
			 action_secondaryDriverTasksImplementation.cpp)
SET (HEADERS action_secondaryDriverTasks.h
			 action_secondaryDriverTasksGlobal.h
			 action_secondaryDriverTasksImplementation.h)

add_definitions(-DACTION_SECONDARY_DRIVER_TASKS_LIBRARY)

OPENPASS_add_component(Action_SecondaryDriverTasks "")
