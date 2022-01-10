add_openpass_target(
  NAME TUDresdenCommon TYPE library LINKAGE shared COMPONENT common
 
  HEADERS
    MentalInfrastructure/Element.h
    MentalInfrastructure/Intersection.h
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
 
  SOURCES
    MentalInfrastructure/Element.cpp
    MentalInfrastructure/Intersection.cpp
    MentalInfrastructure/Lane.cpp
    MentalInfrastructure/Road.cpp
    MentalInfrastructure/Section.cpp
    MentalInfrastructure/Trafficsign.cpp
    MentalInfrastructure/RoadmapGraph/roadmap_graph.cpp
    Helper.cpp
    PerceptionData.cpp    
    WorldRepresentation.cpp
    libs/magic_enum.hpp
 
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
 
  LIBRARIES
    Common
)