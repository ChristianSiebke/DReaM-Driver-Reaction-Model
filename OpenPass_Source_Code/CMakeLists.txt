if(WITH_SIMCORE)
  add_subdirectory(openPASS)
endif()

if(WITH_GUI)
  add_subdirectory(openPASS/Common)
  add_subdirectory(openPASS/CoreFramework/CoreShare)
  add_subdirectory(openPASS_GUI)
endif()
