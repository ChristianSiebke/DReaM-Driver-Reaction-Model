set(PYOPENPASS_SPECIALIZED_TEST_SCOPES
  ADAS_AEB
  ADAS_AEB_InCurve
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

set(PYOPENPASS_GENERIC_TEST_SCOPES
  generic
)

set(PYOPENPASS_GENERIC_TEST_SCOPES_WIN64
  generic_fmu_win64
)

set(PYOPENPASS_GENERIC_TEST_SCOPES_LINUX64
  generic_fmu_linux64
)

if(WIN32)
  list(APPEND PYOPENPASS_GENERIC_TEST_SCOPES ${PYOPENPASS_GENERIC_TEST_SCOPES_WIN64})
else()
  list(APPEND PYOPENPASS_GENERIC_TEST_SCOPES ${PYOPENPASS_GENERIC_TEST_SCOPES_LINUX64})
endif()

set(RESOURCES ${CMAKE_CURRENT_LIST_DIR}/../endToEndTests/Resources)
list(PREPEND PYOPENPASS_SPECIALIZED_TEST_SCOPES $ENV{ADDITIONAL_ENDTOEND_TEST_SCOPES})

# this is the main pyOpenPASS target
add_custom_target(pyOpenPASS
  COMMAND ./report.py -r ${RESOURCES}/Configurations -c end_to_end.json --scope ${PYOPENPASS_GENERIC_TEST_SCOPES} ${PYOPENPASS_SPECIALIZED_TEST_SCOPES} --results ${CMAKE_INSTALL_PREFIX}/results --output ${CMAKE_INSTALL_PREFIX}/testreport
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

# add a target for every generic scope and specialized scope, respectively
foreach(SCOPE IN LISTS PYOPENPASS_GENERIC_TEST_SCOPES PYOPENPASS_SPECIALIZED_TEST_SCOPES)
  add_custom_target(pyOpenPASS_${SCOPE}
    COMMAND ./main.py --slave ${CMAKE_INSTALL_PREFIX} --mutual ${RESOURCES}/DefaultConfigurations --resources ${RESOURCES}/Configurations --config end_to_end.json --scope ${SCOPE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  )
  add_dependencies(pyOpenPASS pyOpenPASS_${SCOPE})
endforeach()

