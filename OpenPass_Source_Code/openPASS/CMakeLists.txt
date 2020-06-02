

set(FOLDER "Core")
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/Interfaces)

add_subdirectory(Common)
add_subdirectory(Components)
add_subdirectory(CoreFramework)
add_subdirectory(CoreModules)

if(BUILD_PCM)
    add_subdirectory(Common_PCM)
    add_subdirectory(Components_PCM)
    add_subdirectory(CoreModules_PCM)
endif()
