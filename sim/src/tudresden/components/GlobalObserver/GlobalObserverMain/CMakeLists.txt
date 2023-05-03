set(FOLDER "GlobalObserverMain")
set(COMPONENT_NAME GlobalObserverMain)

add_compile_definitions(GlobalObserverMain_EXPORTS)
 
add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT common
 
  HEADERS
    # MAIN
    GlobalObserver_main.h
    ../Calculators/ConflictAreaCalculator.h
    ../Calculators/StoppingPointCalculator.h
    ../Calculators/RoadmapGraphCalculator.h
    ../Converters/RoadNetworkConverter.h
    ../Converters/AgentPerceptionConverter.h
    ../Routes/RouteConverter.h
  SOURCES
    # MAIN
    GlobalObserver_main.cpp
    ../Calculators/ConflictAreaCalculator.cpp
    ../Calculators/StoppingPointCalculator.cpp
    ../Calculators/RoadmapGraphCalculator.cpp
    ../Converters/RoadNetworkConverter.cpp
    ../Converters/AgentPerceptionConverter.cpp
    ../Routes/RouteConverter.cpp
    ../Routes/RouteImporter.cpp
    ../../../../core/opSimulation/modules/World_OSI/WorldDataQuery.cpp
    ../../../../core/opSimulation/modules/World_OSI/RoadStream.cpp
    ../../../../core/opSimulation/modules/World_OSI/LaneStream.cpp
    ../../DriverReactionModel/Logger.cpp
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
    ../../../../core/opSimulation/modules/World_OSI
    ../../DriverReactionModel/
    ..
    .
  LIBRARIES
    Qt5::Core
    Common
    TUDresdenCommon
    World_OSI
    
  LINKOSI
)
