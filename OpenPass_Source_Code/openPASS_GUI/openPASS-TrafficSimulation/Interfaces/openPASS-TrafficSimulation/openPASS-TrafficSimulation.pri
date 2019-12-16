!contains(INCLUDEPATH, $$clean_path($$PWD/..)) {
    INCLUDEPATH += $$clean_path($$PWD/..)
}

HEADERS += \
    $$PWD/TrafficSimulationInterface.h \
    $$PWD/ExperimentInterface.h \
    $$PWD/EnvironmentInterface.h \
    $$PWD/EnvironmentItemInterface.h \
    $$PWD/ScenarioInterface.h \
    $$PWD/TrafficInterface.h \
    $$PWD/TrafficItemInterface.h \
    $$PWD/TrafficItemMapInterface.h \
