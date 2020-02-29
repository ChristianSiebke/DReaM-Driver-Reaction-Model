SET (SOURCES )
SET (HEADERS Interfaces/openPASS/PluginInterface.h 
					  Interfaces/openPASS/PluginManagerInterface.h
					  Interfaces/openPASS/RandomInterface.h
					  Interfaces/openPASS/ioplugininterface.h
					  Interfaces/openPASS/ServiceInterface.h)

OPENPASS_ADD_LIBRARY(openPASSInterfaces SHARED)
set_target_properties(openPASSInterfaces PROPERTIES FOLDER "GUI")
qt5_use_modules(openPASSInterfaces Core Gui Widgets Xml)


SET (SOURCES openPASS.cpp
             Models/PluginManagerModel.cpp
			 Models/ServiceManagerModel.cpp)
SET (HEADERS Models/PluginManagerModel.h
		     Models/ServiceManagerModel.h)

INCLUDE_DIRECTORIES(.)

OPENPASS_add_executable(openPASS openPASS.rc)
set_target_properties(openPASS PROPERTIES FOLDER "GUI")

target_link_libraries(openPASS CoreShare openPASSInterfaces)

qt5_use_modules(openPASS Core Gui Widgets Xml)