set(COMPONENT_NAME ALGORITHM_DReaM)
 
add_compile_definitions(ALGORITHM_DReaM_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module
  
  HEADERS
    agentstaterecorder.h
    Algorithm_DReaM.h
    Algorithm_DReaMGlobal.h
    Algorithm_DReaMImplementation.h
    DriverReactionModel.h
    Logger.h
    LoggerInterface.h
    Components/ComponentInterface.h
    Components/Navigation.h
    Components/ActionDecision/ActionDecision.h
    Components/ActionDecision/ActionStateHandler.h
    Components/ActionDecision/Anticipation.h
    Components/ActionDecision/VelocityCalculation.h
    Components/CognitiveMap/CognitiveMap.h
    Components/CognitiveMap/CommandInterface.h
    Components/CognitiveMap/Memory.h
    Components/CognitiveMap/ReactionTime.h
    Components/CognitiveMap/Interpreter/CollisionInterpreter.h
    Components/CognitiveMap/Interpreter/CrossingInfoInterpreter.h
    Components/CognitiveMap/Interpreter/FollowingInterpreter.h
    Components/CognitiveMap/Interpreter/InterpreterInterface.h
    Components/CognitiveMap/Interpreter/RightOfWayInterpreter.h
    Components/CognitiveMap/Interpreter/WorldInterpreter.h
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/RightBeforeLeftRegulation.h
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/RightOfWayRegulationInterface.h
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/TrafficLightRegulation.h
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/TrafficSignRegulation.h
    Components/GazeMovement/GazeMovement.h
    Components/GazeMovement/RoadSegments/RoadSegmentInterface.h
    Components/GazeMovement/RoadSegments/StandardRoad.h
    Components/GazeMovement/RoadSegments/XJunction.h
    Components/Importer/BehaviourConverter.h
    Components/Importer/BehaviourImporter.h
    Components/TrafficSignMemory/TrafficSignMemory.h

  SOURCES
    agentstaterecorder.cpp
    Algorithm_DReaM.cpp
    Algorithm_DReaMImplementation.cpp
    DriverReactionModel.cpp
    Logger.cpp
    Components/Navigation.cpp
    Components/ActionDecision/ActionDecision.cpp
    Components/ActionDecision/ActionStateHandler.cpp
    Components/ActionDecision/Anticipation.cpp
    Components/ActionDecision/VelocityCalculation.cpp
    Components/CognitiveMap/CognitiveMap.cpp
    Components/CognitiveMap/Memory.cpp
    Components/CognitiveMap/ReactionTime.cpp
    Components/CognitiveMap/Interpreter/CollisionInterpreter.cpp
    Components/CognitiveMap/Interpreter/CrossingInfoInterpreter.cpp
    Components/CognitiveMap/Interpreter/FollowingInterpreter.cpp
    Components/CognitiveMap/Interpreter/RightOfWayInterpreter.cpp
    Components/CognitiveMap/Interpreter/WorldInterpreter.cpp
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/RightBeforeLeftRegulation.cpp
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/RightOfWayRegulationInterface.cpp
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/TrafficLightRegulation.cpp
    Components/CognitiveMap/Interpreter/RightOfWayRegulation/TrafficSignRegulation.cpp
    Components/GazeMovement/GazeMovement.cpp
    Components/GazeMovement/RoadSegments/RoadSegmentInterface.cpp
    Components/GazeMovement/RoadSegments/StandardRoad.cpp
    Components/GazeMovement/RoadSegments/XJunction.cpp
    Components/Importer/BehaviourConverter.cpp
    Components/Importer/BehaviourImporter.cpp
    Components/TrafficSignMemory/TrafficSignMemory.cpp
    ../../../common/xmlParser.cpp

  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
    ../../common
    .
 
  LIBRARIES
    Qt5::Core
    Qt5::Xml
    Common
    TUDresdenCommon
)