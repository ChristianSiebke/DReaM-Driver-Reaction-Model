set(COMPONENT_NAME DriverReactionModel)
 
add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared COMPONENT module
 
#   HEADERS
 
#   SOURCES
 
  INCDIRS
    ${OPENPASS_SIMCORE_DIR}/core
    ${OPENPASS_SIMCORE_DIR}/common
 
  LIBRARIES
    Qt5::Core
    Common
    TUDresdenCommon
)