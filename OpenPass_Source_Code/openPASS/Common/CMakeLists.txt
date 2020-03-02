add_openpass_target(
  NAME Common TYPE library LINKAGE static
  
  HEADERS
    eventDetectorDefinitions.h
    vector2d.h
    vector3d.h

  SOURCES
    vector2d.cpp
    vector3d.cpp
    eventDetectorDefinitions.cpp
)
