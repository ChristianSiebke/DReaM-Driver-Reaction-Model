set(COMPONENT_NAME BasicDataStore)

add_compile_definitions(BASIC_DATASTORE_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module

  HEADERS
    basicDataStore.h
    basicDataStoreGlobal.h
    basicDataStoreImplementation.h

  SOURCES
    basicDataStore.cpp
    basicDataStoreImplementation.cpp

  LIBRARIES
    Qt5::Core
)

