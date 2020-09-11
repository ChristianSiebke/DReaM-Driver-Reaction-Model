set(COMPONENT_NAME SignalPrioritizer)

add_compile_definitions(SIGNAL_PRIORITIZER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    signalPrioritizer.h
    src/signalPrioritizerImpl.h

  SOURCES
    signalPrioritizer.cpp
    src/signalPrioritizerImpl.cpp

  LIBRARIES
    Qt5::Core
)
