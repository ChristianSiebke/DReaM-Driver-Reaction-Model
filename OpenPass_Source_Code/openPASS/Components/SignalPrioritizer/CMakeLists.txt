set(COMPONENT_NAME SignalPrioritizer)

add_compile_definitions(SIGNAL_PRIORITIZER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    signalPrioritizer.h
    signalPrioritizerGlobal.h
    signalPrioritizerImplementation.h

  SOURCES
    signalPrioritizer.cpp
    signalPrioritizerImplementation.cpp

  LIBRARIES
    Qt5::Core
)
