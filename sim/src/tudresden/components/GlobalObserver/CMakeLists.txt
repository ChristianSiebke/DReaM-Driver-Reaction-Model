set(FOLDER "GlobalObserver")
set(COMPONENT_NAME GlobalObserver)

add_compile_definitions(GLOBALOBSERVER_LIBRARY)
 
add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module
 
  HEADERS
    # MAIN
    GlobalObserver_global.h
    GlobalObserver.h
    GlobalObserver_implementation.h
    Calculators/ConflictAreaCalculator.h
    Calculators/StoppingPointCalculator.h
    Calculators/RoadmapGraphCalculator.h
    Converters/RoadNetworkConverter.h
    Converters/AgentPerceptionConverter.h
    Routes/RouteConverter.h
  SOURCES
    # MAIN
    GlobalObserver.cpp
    GlobalObserver_implementation.cpp
    Calculators/ConflictAreaCalculator.cpp
    Calculators/StoppingPointCalculator.cpp
    Calculators/RoadmapGraphCalculator.cpp
    Converters/RoadNetworkConverter.cpp
    Converters/AgentPerceptionConverter.cpp
    Routes/RouteConverter.cpp
    Routes/RouteImporter.cpp
    ../../../core/opSimulation/modules/World_OSI/WorldDataQuery.cpp
    ../../../core/opSimulation/modules/World_OSI/RoadStream.cpp
    ../../../core/opSimulation/modules/World_OSI/LaneStream.cpp
    ../DriverReactionModel/Logger.cpp
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
    ../../../core/opSimulation/modules/World_OSI
    ../../../common/RoutePlanning
    ../DriverReactionModel/
    .
  LIBRARIES
    Qt5::Core
    Common
    TUDresdenCommon
    World_OSI
    GlobalObserverMain
    SimulationCore
    Sensor_Perception
    AnalysisDataRecorder
    
  LINKOSI
)
add_subdirectory(GlobalObserverMain) 
add_subdirectory(Analytics) 

