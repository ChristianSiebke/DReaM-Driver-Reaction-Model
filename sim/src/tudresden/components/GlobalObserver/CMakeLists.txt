set(COMPONENT_NAME GlobalObserver)

add_compile_definitions(GLOBALOBSERVER_LIBRARY)
 
add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module
 
  HEADERS
    # MAIN
    GlobalObserver_global.h
    GlobalObserver.h
    GlobalObserver_implementation.h
  SOURCES
    # MAIN
    GlobalObserver.cpp
    GlobalObserver_implementation.cpp
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
    ../../../core/opSimulation/modules/World_OSI
    .
  LIBRARIES
    Qt5::Core
    Common
    TUDresdenCommon
    World_OSI
    SimulationCore
    Sensor_Perception
    
  LINKOSI
)