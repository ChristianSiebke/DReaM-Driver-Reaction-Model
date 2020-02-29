
# All includes should be relative to the openPASS directory or relative to the current file
include_directories(OpenPASS OpenPASS/Common OpenPASS/Interfaces)

ADD_SUBDIRECTORY(openPASS)
ADD_SUBDIRECTORY(openPASS_GUI)

file(COPY ${CMAKE_SOURCE_DIR}/OpenPass_Source_Code/openPASS_Resource/OpenPass_OSI_UseCase/AEB/configs DESTINATION ${CMAKE_BINARY_DIR}/OpenPASS)