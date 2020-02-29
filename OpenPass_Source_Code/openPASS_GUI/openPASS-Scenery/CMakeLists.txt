SET (SOURCES SceneryPlugin.cpp
Models/SceneryModel.cpp
Views/SceneryView.cpp
Presenters/SceneryPresenter.cpp
)
SET (HEADERS SceneryPlugin.h 
Models/SceneryModel.h
Views/SceneryView.h
Presenters/SceneryPresenter.h
)

SET(UIS 
Views/SceneryView.ui
)


INCLUDE_DIRECTORIES(.)

add_definitions(-DScenery_LIBRARY)

OPENPASS_GUI_add_plugin(Scenery "")

target_link_libraries(Scenery Project openPASSInterfaces)
