set(COMPONENT_NAME Stochastics)

add_compile_definitions(STOCHASTICS_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    stochastics.h
    stochastics_implementation.h
    stochastics_global.h

  SOURCES
    stochastics.cpp
    stochastics_implementation.cpp

  LIBRARIES
    Qt5::Core
)
