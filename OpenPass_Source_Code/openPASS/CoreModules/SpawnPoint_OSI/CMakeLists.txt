SET (SOURCES SpawnPoint.cpp
			SpawnPointExport.cpp
			SpawnPointSceneryParser.cpp)
SET (HEADERS SpawnPoint.h
			SpawnPointExport.h
			SpawnPointGlobal.h
			SpawnPointSceneryParser.h)


add_definitions(-DSPAWNPOINT_LIBRARY)
include_directories(. ${OSI_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})

OPENPASS_ADD_MODULE(SpawnPoint_OSI OSI)
qt5_use_modules(SpawnPoint_OSI Xml)


target_link_libraries(SpawnPoint_OSI Common  ${OSI_LIBRARIES} ${PROTOBUF_LIBRARIES})


