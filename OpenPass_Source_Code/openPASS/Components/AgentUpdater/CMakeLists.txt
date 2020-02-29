SET (SOURCES agentUpdater.cpp
			 agentUpdaterImplementation.cpp)
SET (HEADERS agentUpdater.h
			 agentUpdaterGlobal.h
			 agentUpdaterImplementation.h)

add_definitions(-DAGENT_UPDATER_LIBRARY)

OPENPASS_add_component(AgentUpdater "")
