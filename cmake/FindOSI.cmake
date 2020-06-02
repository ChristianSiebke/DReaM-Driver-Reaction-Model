# - Find OSI
# Find the OSI includes and library
#
#  OSI_INCLUDE_DIR - Where to find OSI includes
#  OSI_LIBRARIES   - List of libraries when using OSI
#  OSI_FOUND       - True if OSI was found

IF(OSI_INCLUDE_DIR)
  SET(OSI_FIND_QUIETLY TRUE)
ENDIF(OSI_INCLUDE_DIR)

FIND_PATH(OSI_INCLUDE_DIR "osi3/osi_version.pb.h"
  PATHS
  ${PREFIX_PATH}
  $ENV{OSI_HOME}/include
  $ENV{EXTERNLIBS}/OSI/include
  /usr/local/include
  /usr/include
   DOC "OSI - Headers"
)

SET(OSI_NAMES
    osi3/open_simulation_interface.lib 
    osi3/libopen_simulation_interface.dll.a
    osi3/libopen_simulation_interface_pic.lib
    osi3/libopen_simulation_interface.so
)

SET(OSI_DBG_NAMES
    osi3/open_simulation_interfaced.lib
    osi3/libopen_simulation_interfaced.dll.a
    osi3/libopen_simulation_interface_picd.lib
)

FIND_LIBRARY(OSI_LIBRARY NAMES ${OSI_NAMES}
  PATHS
  ${PREFIX_PATH}
  $ENV{OSI_HOME}
  $ENV{EXTERNLIBS}/OSI
  /usr/local
  /usr
  PATH_SUFFIXES lib lib64
  DOC "OSI - Library"
)

INCLUDE(FindPackageHandleStandardArgs)

IF(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(OSI_LIBRARY_DEBUG NAMES ${OSI_DBG_NAMES}
    PATHS
    ${PREFIX_PATH}
    $ENV{OSI_HOME}/lib
    $ENV{EXTERNLIBS}/OSI/lib
    DOC "OSI - Library (Debug)"
  )
  
  IF(OSI_LIBRARY_DEBUG AND OSI_LIBRARY)
    SET(OSI_LIBRARIES optimized ${OSI_LIBRARY} debug ${OSI_LIBRARY_DEBUG})
  ENDIF(OSI_LIBRARY_DEBUG AND OSI_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSI DEFAULT_MSG OSI_LIBRARY OSI_LIBRARY_DEBUG OSI_INCLUDE_DIR)

  MARK_AS_ADVANCED(OSI_LIBRARY OSI_LIBRARY_DEBUG OSI_INCLUDE_DIR)
  
ELSE(MSVC)
  # rest of the world
  SET(OSI_LIBRARIES ${OSI_LIBRARY})

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSI DEFAULT_MSG OSI_LIBRARY OSI_INCLUDE_DIR)
  
  MARK_AS_ADVANCED(OSI_LIBRARY OSI_INCLUDE_DIR)
  
ENDIF(MSVC)
