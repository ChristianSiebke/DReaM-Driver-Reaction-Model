set(COMPONENT_NAME BasicDataBuffer)

add_compile_definitions(BASIC_DATABUFFER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT core

  HEADERS
    basicDataBuffer.h
    basicDataBufferGlobal.h
    basicDataBufferImplementation.h

  SOURCES
    basicDataBuffer.cpp
    basicDataBufferImplementation.cpp

  LIBRARIES
    Qt5::Core
)

