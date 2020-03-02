set(OPENPASS_SIMCORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../OpenPass_Source_Code/openPASS)
set(TEST_PATH ${CMAKE_CURRENT_LIST_DIR})

include_directories(
  ${TEST_PATH}
  ${TEST_PATH}/common
  ${TEST_PATH}/common/gtest
  ${TEST_PATH}/fakes/gmock
  ${OPENPASS_SIMCORE_DIR}
)

add_subdirectory(unitTests)
add_subdirectory(integrationTests)
