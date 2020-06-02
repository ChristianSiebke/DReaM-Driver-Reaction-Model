set(COMPONENT_NAME Action_SecondaryDriverTasks)

add_compile_definitions(ACTION_SECONDARY_DRIVER_TASKS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    action_secondaryDriverTasks.h
    action_secondaryDriverTasksGlobal.h
    action_secondaryDriverTasksImplementation.h

  SOURCES
    action_secondaryDriverTasks.cpp
    action_secondaryDriverTasksImplementation.cpp

  LIBRARIES
    Qt5::Core
)
