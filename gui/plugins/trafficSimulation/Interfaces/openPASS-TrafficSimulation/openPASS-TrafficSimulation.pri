################################################################################
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

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
