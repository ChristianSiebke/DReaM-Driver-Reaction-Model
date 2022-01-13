add_openpass_target(
  NAME TUDresdenCommon TYPE library LINKAGE shared COMPONENT common
 
  HEADERS
    MentalInfrastructure/Element.h
    MentalInfrastructure/Junction.h
    MentalInfrastructure/Lane.h
    MentalInfrastructure/Road.h
    MentalInfrastructure/Section.h
    MentalInfrastructure/Trafficsign.h
    MentalInfrastructure/RoadmapGraph/roadmap_graph.h
    Definitions.h
    Helper.h
    PerceptionData.h
    threading/ThreadSafeContainer.h
    WorldRepresentation.h
    RoadSegmentInterface.h
    BehaviourData.h
 
  SOURCES
    MentalInfrastructure/Element.cpp
    MentalInfrastructure/Junction.cpp
    MentalInfrastructure/Lane.cpp
    MentalInfrastructure/Road.cpp
    MentalInfrastructure/Section.cpp
    MentalInfrastructure/Trafficsign.cpp
    MentalInfrastructure/RoadmapGraph/roadmap_graph.cpp
    Helper.cpp
    PerceptionData.cpp    
    WorldRepresentation.cpp
    libs/magic_enum.hpp
    RoadSegmentInterface.cpp
 
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
    ../../core/opSimulation/framework/
 
  LIBRARIES
    Common
)