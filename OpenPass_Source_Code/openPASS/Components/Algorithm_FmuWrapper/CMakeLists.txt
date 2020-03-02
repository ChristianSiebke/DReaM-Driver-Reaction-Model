set(COMPONENT_NAME Algorithm_FmuWrapper)

add_compile_definitions(ALGORITHM_FMUWRAPPER_LIBRARY)

add_openpass_target(
  NAME ${COMPONENT_NAME} TYPE library LINKAGE shared

  HEADERS
    AlgorithmFmuWrapper.h
    AlgorithmFmuWrapperGlobal.h
    AlgorithmFmuWrapperImplementation.h
    OsmpFmuHandler.h

  SOURCES
    FmiImporter/src/Common/fmuChecker.c
    FmiImporter/src/FMI1/fmi1_check.c
    FmiImporter/src/FMI1/fmi1_cs_sim.c
    FmiImporter/src/FMI1/fmi1_me_sim.c
    FmiImporter/src/FMI2/fmi2_check.c
    FmiImporter/src/FMI2/fmi2_cs_sim.c
    FmiImporter/src/FMI2/fmi2_me_sim.c
    AlgorithmFmuWrapper.cpp
    AlgorithmFmuWrapperImplementation.cpp
    OsmpFmuHandler.cpp

  INCDIRS
    ${FMILibrary_INCLUDE_DIR}
    ${FMILibrary_INCLUDE_DIR}/FMI
    ${FMILibrary_INCLUDE_DIR}/FMI1
    ${FMILibrary_INCLUDE_DIR}/FMI2
    ${FMILibrary_INCLUDE_DIR}/JM
    FmiImporter/include
    ../../CoreModules/World_OSI

  LIBRARIES
    Qt5::Core
    Boost::filesystem
    ${FMILibrary_LIBRARY_DIR}
    Common

  LINKOSI
)
