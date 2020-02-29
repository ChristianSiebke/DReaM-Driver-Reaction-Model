SET (SOURCES WindowPlugin.cpp
Models/WindowModel.cpp
Views/WindowView.cpp
Views/LogoDelegate.cpp
Presenters/WindowPresenter.cpp
)
SET (HEADERS WindowPlugin.h 
Models/WindowModel.h
Views/WindowView.h
Views/LogoDelegate.h
Presenters/WindowPresenter.h
Interfaces/openPASS-Window/WindowInterface.h
)

SET(UIS 
Views/WindowView.ui
)

INCLUDE_DIRECTORIES(.)


add_definitions(-DWindow_LIBRARY)

OPENPASS_GUI_add_plugin(Window "")

target_link_libraries(Window openPASSInterfaces)