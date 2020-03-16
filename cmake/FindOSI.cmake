#/*******************************************************************************
#* Copyright (c) 2020 HLRS, University of Stuttgart
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/
# - Find OSI
# Find the OSI includes and library
#
#  OSI_INCLUDE_DIR - Where to find OSI includes
#  OSI_LIBRARIES   - List of libraries when using OSI
#  OSI_FOUND       - True if OSI was found

if(OSI_INCLUDE_DIR)
  set(OSI_FIND_QUIETLY TRUE)
endif(OSI_INCLUDE_DIR)

FIND_PATH(OSI_INCLUDE_DIR "osi3/osi_version.pb.h"
  PATHS
  $ENV{OSI_HOME}/include
  $ENV{EXTERNLIBS}/OSI/include
  ~/Library/Frameworks/include
  /Library/Frameworks/include
  /usr/local/include
  /usr/include
  /sw/include # Fink
  /opt/local/include # DarwinPorts
  /opt/csw/include # Blastwave
  /opt/include
  DOC "OSI - Headers"
)

set(OSI_NAMES osi3/open_simulation_interface.lib osi3/open_simulation_interface_pic.lib osi/open_simulation_interface_pic.lib)
set(OSI_DBG_NAMES osi3/open_simulation_interfaced.lib osi3/open_simulation_interface_picd.lib osi/open_simulation_interface_picd.lib)

FIND_LIBRARY(OSI_LIBRARY NAMES ${OSI_NAMES}
  PATHS
  $ENV{OSI_HOME}
  $ENV{EXTERNLIBS}/OSI
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "OSI - Library"
)

include(FindPackageHandleStandardArgs)

if(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(OSI_LIBRARY_DEBUG NAMES ${OSI_DBG_NAMES}
    PATHS
    $ENV{OSI_HOME}/lib
    $ENV{EXTERNLIBS}/OSI/lib
    DOC "OSI - Library (Debug)"
  )
  
  if(OSI_LIBRARY_DEBUG AND OSI_LIBRARY)
    set(OSI_LIBRARIES optimized ${OSI_LIBRARY} debug ${OSI_LIBRARY_DEBUG})
  endif(OSI_LIBRARY_DEBUG AND OSI_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSI DEFAULT_MSG OSI_LIBRARY OSI_LIBRARY_DEBUG OSI_INCLUDE_DIR)

  MARK_AS_ADVANCED(OSI_LIBRARY OSI_LIBRARY_DEBUG OSI_INCLUDE_DIR)
  
else(MSVC)
  # rest of the world
  set(OSI_LIBRARIES ${OSI_LIBRARY})

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSI DEFAULT_MSG OSI_LIBRARY OSI_INCLUDE_DIR)
  
  MARK_AS_ADVANCED(OSI_LIBRARY OSI_INCLUDE_DIR)
  
endif(MSVC)

if(OSI_FOUND)
  set(OSI_INCLUDE_DIRS ${OSI_INCLUDE_DIR})
endif(OSI_FOUND)
