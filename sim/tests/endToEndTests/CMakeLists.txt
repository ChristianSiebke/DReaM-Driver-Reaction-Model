################################################################################
# Copyright (c) 2020, 2021 in-tech GmbH
#               2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# Default pyOpenPASS config file to use
set(PYOPENPASS_DEFAULT_CONFIG ${CMAKE_CURRENT_LIST_DIR}/end_to_end.json)

# Default resource directory
set(PYOPENPASS_DEFAULT_RESOURCES ${CMAKE_CURRENT_LIST_DIR}/../../contrib/examples)

# Selection of specialized tests to run. For each entry a scope has to be defined in the PYOPENPASS_DEFAULT_CONFIG file.
set(PYOPENPASS_SPECIALIZED_TEST_SCOPES
  ADAS_AEB
  ADAS_AEB_InCurve
  ADAS_AEB_CutIn_Triggers
  ADAS_AEB_CutIn_NoCollision
  ByEntityCondition
  OSCAction_DoubleLaneChange
  OSCAction_RemoveAgent
  OSCAction_SingleLaneChange
  OSCAction_StochasticTCoordinate
  OSCAction_StochasticVelocityEgo
  OSCAction_StochasticXPosition
  LocalizationOnJunction
  Sensor_Latency
)

# Generic test scopes
set(PYOPENPASS_GENERIC_TEST_SCOPES
  generic
)

# Platform specific test scopes (Windows)
set(PYOPENPASS_GENERIC_TEST_SCOPES_WIN64
)

# Platform specific test scopes (Linux)
set(PYOPENPASS_GENERIC_TEST_SCOPES_LINUX64
)

####################

if(WIN32)
  list(APPEND PYOPENPASS_GENERIC_TEST_SCOPES ${PYOPENPASS_GENERIC_TEST_SCOPES_WIN64})
elseif(UNIX)
  list(APPEND PYOPENPASS_GENERIC_TEST_SCOPES ${PYOPENPASS_GENERIC_TEST_SCOPES_LINUX64})
endif()

# Find python3 interpreter
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# This is the main pyOpenPASS target. It will create a HTML report of the individual tests.
add_custom_target(pyOpenPASS
  COMMAND ${Python3_EXECUTABLE} report.py -r ${PYOPENPASS_DEFAULT_RESOURCES}/Configurations $<$<BOOL:${PYOPENPASS_EXTRA_RESOURCES}>:${PYOPENPASS_EXTRA_RESOURCES}/Configurations> -c ${PYOPENPASS_DEFAULT_CONFIG} ${PYOPENPASS_EXTRA_CONFIG} --scope ${PYOPENPASS_GENERIC_TEST_SCOPES} ${PYOPENPASS_SPECIALIZED_TEST_SCOPES} ${PYOPENPASS_EXTRA_TEST_SCOPES} --results ${CMAKE_INSTALL_PREFIX}/results --output ${CMAKE_INSTALL_PREFIX}/testreport
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/pyOpenPASS
)

# Add a target for every generic scope and specialized scope, respectively.
# This also adds a dependency on the main pyOpenPASS target specified above.
foreach(SCOPE IN LISTS PYOPENPASS_GENERIC_TEST_SCOPES PYOPENPASS_SPECIALIZED_TEST_SCOPES)
  add_custom_target(pyOpenPASS_${SCOPE}
    COMMAND ${Python3_EXECUTABLE} main.py --slave ${CMAKE_INSTALL_PREFIX} --mutual ${PYOPENPASS_DEFAULT_RESOURCES}/DefaultConfigurations --resources ${PYOPENPASS_DEFAULT_RESOURCES}/Configurations --config ${PYOPENPASS_DEFAULT_CONFIG} --scope ${SCOPE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/pyOpenPASS
  )
  add_dependencies(pyOpenPASS pyOpenPASS_${SCOPE})
endforeach()

# Add a target for every extra scope. This can be used to inject tests from a different source tree by setting these variables before including this CMakeLists file:
#   - PYOPENPASS_EXTRA_CONFIG      ... JSON file specifying additional scopes
#   - PYOPENPASS_EXTRA_RESOURCES   ... Directory containing extra configurations
#   - PYOPENPASS_EXTRA_TEST_SCOPES ... List of selected scopes to run
# This also adds a dependency on the main pyOpenPASS target specified above.
foreach(SCOPE IN LISTS PYOPENPASS_EXTRA_TEST_SCOPES)
  add_custom_target(pyOpenPASS_${SCOPE}
    COMMAND ${Python3_EXECUTABLE} main.py --slave ${CMAKE_INSTALL_PREFIX} --mutual ${PYOPENPASS_EXTRA_RESOURCES}/DefaultConfigurations --resources ${PYOPENPASS_EXTRA_RESOURCES}/Configurations --config ${PYOPENPASS_EXTRA_CONFIG} --scope ${SCOPE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/pyOpenPASS
  )
  add_dependencies(pyOpenPASS pyOpenPASS_${SCOPE})
endforeach()
