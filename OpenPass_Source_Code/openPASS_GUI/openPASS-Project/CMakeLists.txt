SET (SOURCES ProjectPlugin.cpp
Models/ProjectXMLSaveModel.cpp
Models/ProjectModel.cpp
Models/ProjectXMLLoadModel.cpp
Models/ProjectXMLBaseModel.cpp
Views/ProjectView.cpp
Presenters/ProjectPresenter.cpp
)
SET (HEADERS ProjectPlugin.h 
Models/ProjectModel.h
Models/ProjectXMLLoadModel.h
Models/ProjectXMLSaveModel.h
Models/ProjectXMLBaseModel.h
Views/ProjectView.h
Presenters/ProjectPresenter.h
Interfaces/openPASS-Project/ProjectInterface.h
)

SET(UIS 
Views/ProjectView.ui
)

INCLUDE_DIRECTORIES(.)

add_definitions(-DProject_LIBRARY)

OPENPASS_GUI_add_plugin(Project "")

target_link_libraries(Project openPASSInterfaces)

