set(COMPONENT_NAME Action_SecondaryDriverTasks)

add_compile_definitions(ACTION_SECONDARY_DRIVER_TASKS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    action_secondaryDriverTasks.h
    src/secondaryDriverTasks.h

  SOURCES
    action_secondaryDriverTasks.cpp
    src/secondaryDriverTasks.cpp

  LIBRARIES
    Qt5::Core
)
