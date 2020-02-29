SET (SOURCES  framework/main.cpp
			  framework/processManager.cpp
			  importer/masterConfigImporter.cpp)
			  
SET (HEADERS framework/processManager.h
			 framework/masterConfig.h
			 framework/slaveConfig.h
			 openPassMaster.h
			 importer/masterConfigImporter.h)
			 
OPENPASS_add_executable(OpenPassMaster )
set_target_properties(OpenPassMaster PROPERTIES FOLDER "CoreFramework")

target_link_libraries(OpenPassMaster CoreShare)

qt5_use_modules(OpenPassMaster Widgets Xml Concurrent)