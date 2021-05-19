set(COMPONENT_NAME CoreCommon)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE static COMPONENT bin

  HEADERS
    callbacks.h
    coreDataPublisher.h
    timeKeeper.h
    ../../../../common/xmlParser.h
    cephesMIT/mconf.h

  SOURCES
    callbacks.cpp
    coreDataPublisher.cpp
    ../../../../common/xmlParser.cpp
    cephesMIT/const.c
    cephesMIT/fresnl.c
    cephesMIT/polevl.c

  LIBRARIES
    Qt5::Core
    Qt5::Xml

  INCDIRS
    ../../../..
)

set_target_properties(CoreCommon PROPERTIES FOLDER "core")
