# - Find FMILibrary
# Find the FMILibrary includes and library
#
#  FMILibrary_INCLUDE_DIRS  - Where to find FMILibrary includes
#  FMILibrary_LIBRARIES     - List of libraries
#  FMILibrary_FOUND         - True if FMILibrary was found

if(FMILibrary_INCLUDE_DIR)
  set(FMILibrary_FIND_QUIETLY TRUE)
endif()

find_path(FMILibrary_INCLUDE_DIR "fmilib.h"
  PATHS
  ${PREFIX_PATH}
  /usr/local/include
  /usr/include
  DOC "FMILibrary - Headers"
)

set(FMILibrary_NAMES
    libfmilib_shared.dll.a
    fmilib.lib
    libfmilib_shared.so
)
set(FMILibrary_DBG_NAMES
    fmilibd.lib
)

find_library(FMILibrary_LIBRARY_DIR NAMES ${FMILibrary_NAMES}
  PATHS
  ${PREFIX_PATH}
  /usr/local
  /usr
  PATH_SUFFIXES lib lib64
  DOC "FMILibrary - Library"
)

include(FindPackageHandleStandardArgs)

if(MSVC)
  # VisualStudio needs a debug version
  find_library(FMILibrary_LIBRARY_DIR_DEBUG NAMES ${FMILibrary_DBG_NAMES}
    PATHS
    ${PREFIX_PATH}
    DOC "FMILibrary - Library (Debug)"
  )
  
  if(FMILibrary_LIBRARY_DIR_DEBUG AND FMILibrary_LIBRARY_DIR)
    set(FMILibrary_LIBRARIES optimized ${FMILibrary_LIBRARY_DIR} debug ${FMILibrary_LIBRARY_DIR_DEBUG})
  endif()

  find_package_handle_standard_args(FMILibrary DEFAULT_MSG FMILibrary_LIBRARY_DIR FMILibrary_LIBRARY_DIR_DEBUG FMILibrary_INCLUDE_DIR)

  mark_as_advanced(FMILibrary_LIBRARY_DIR FMILibrary_LIBRARY_DIR_DEBUG FMILibrary_INCLUDE_DIR)
  
else()
  # rest of the world
  set(FMILibrary_LIBRARIES ${FMILibrary_LIBRARY_DIR})

  find_package_handle_standard_args(FMILibrary DEFAULT_MSG FMILibrary_LIBRARY_DIR FMILibrary_INCLUDE_DIR)
  
  mark_as_advanced(FMILibrary_LIBRARY_DIR FMILibrary_INCLUDE_DIR)
  
endif()
