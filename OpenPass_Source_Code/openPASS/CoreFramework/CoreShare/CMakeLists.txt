set(COMPONENT_NAME CoreShare)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE static COMPONENT bin

  HEADERS
    callbacks.h
    log.h
    parameters.h
    xmlParser.h
    cephesMIT/mconf.h

  SOURCES
    callbacks.cpp
    log.cpp
    parameters.cpp
    xmlParser.cpp
    cephesMIT/const.c
    cephesMIT/fresnl.c
    cephesMIT/polevl.c

  LIBRARIES
    Qt5::Core
    Qt5::Xml
)

set_target_properties(CoreShare PROPERTIES FOLDER "CoreFramework")
