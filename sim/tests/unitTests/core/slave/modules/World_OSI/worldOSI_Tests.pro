# /*********************************************************************
# * Copyright (c) 2019, 2020 in-tech GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

CONFIG += OPENPASS_GTEST \
          OPENPASS_GTEST_DEFAULT_MAIN

include(../../../../../testing.pri)

win32:QMAKE_CXXFLAGS += -Wa,-mbig-obj

UNIT_UNDER_TEST = $$OPEN_SRC/core/slave/modules/World_OSI

# commons
SRC_COMMONS = \
              $$OPEN_SRC/core/common/log.cpp
INC_COMMONS = $$OPEN_SRC/common/vector2d.h \
              $$OPEN_SRC/common/worldDefinitions.h \
              $$OPEN_SRC/../include/worldinterface.h

# owl
SRC_OWL =   \
            $$UNIT_UNDER_TEST/OWL/DataTypes.cpp

INC_OWL =   \
            $$UNIT_UNDER_TEST/OWL/DataTypes.h \
            $$UNIT_UNDER_TEST/OWL/LaneGeometryElement.h \
            $$UNIT_UNDER_TEST/OWL/LaneGeometryJoint.h \
            \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeLaneManager.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeSection.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeLane.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeLaneBoundary.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeRoad.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeOWLJunction.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeTrafficSign.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeRoadMarking.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeMovingObject.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeWorld.h \
            $$UNIT_UNDER_TEST/OWL/fakes/fakeWorldData.h

## world
SRC_WORLD = \
            $$UNIT_UNDER_TEST/AgentAdapter.cpp \
            $$UNIT_UNDER_TEST/AgentNetwork.cpp \
            $$UNIT_UNDER_TEST/egoAgent.cpp \
            $$UNIT_UNDER_TEST/GeometryConverter.cpp \
            $$UNIT_UNDER_TEST/JointsBuilder.cpp \
            $$UNIT_UNDER_TEST/TrafficObjectAdapter.cpp \
            $$UNIT_UNDER_TEST/SceneryConverter.cpp \
            $$UNIT_UNDER_TEST/WorldData.cpp \
            $$UNIT_UNDER_TEST/WorldDataQuery.cpp \
            $$UNIT_UNDER_TEST/WorldDataException.cpp \
            $$UNIT_UNDER_TEST/WorldObjectAdapter.cpp \
            $$UNIT_UNDER_TEST/WorldImplementation.cpp

INC_WORLD = \
            $$UNIT_UNDER_TEST/AgentAdapter.h \
            $$UNIT_UNDER_TEST/AgentNetwork.h \
            $$UNIT_UNDER_TEST/egoAgent.h \
            $$UNIT_UNDER_TEST/GeometryConverter.h \
            $$UNIT_UNDER_TEST/JointsBuilder.h \
            $$UNIT_UNDER_TEST/TrafficObjectAdapter.h \
            $$UNIT_UNDER_TEST/SceneryConverter.h \
            $$UNIT_UNDER_TEST/WorldData.h \
            $$UNIT_UNDER_TEST/WorldDataQuery.h \
            $$UNIT_UNDER_TEST/WorldDataException.h \
            $$UNIT_UNDER_TEST/WorldObjectAdapter.h \
            $$UNIT_UNDER_TEST/WorldImplementation.h

SRC_OTHER = \ # OLD VERSION
            $$UNIT_UNDER_TEST/Localization.cpp \
            $$UNIT_UNDER_TEST/OWL/OpenDriveTypeMapper.cpp


INC_OTHER = \ #OLD_VERSION
            $$UNIT_UNDER_TEST/Localization.h \
            $$UNIT_UNDER_TEST/OWL/OpenDriveTypeMapper.h

# localization
SRC_LOCALIZATION = \
            $$UNIT_UNDER_TEST/WorldToRoadCoordinateConverter.cpp \
            $$UNIT_UNDER_TEST/PointQuery.cpp

INC_LOCALIZATION = \
            $$UNIT_UNDER_TEST/WorldToRoadCoordinateConverter.h \
            $$UNIT_UNDER_TEST/PointQuery.h \
            $$UNIT_UNDER_TEST/LocalizationElement.h

INCLUDEPATH += \
            . \
            $$UNIT_UNDER_TEST \
            $$UNIT_UNDER_TEST/OWL \
            $$UNIT_UNDER_TEST/OWL/fakes \
            $$UNIT_UNDER_TEST/Localization \
            $$OPEN_SRC \
            $$OPEN_SRC/core \
            $$OPEN_SRC/..


SOURCES += \
            $$SRC_COMMONS \
            $$SRC_OWL \
            $$SRC_WORLD \
            $$SRC_OTHER \
            $$SRC_LOCALIZATION \
            \ # GENERATOR-TESTS
            Generators/laneGeometryElementGenerator_Tests.cpp \
            \ # UNIT-TESTS
            geometryConverter_Tests.cpp \
            locator_Tests.cpp \
            pointQuery_Tests.cpp \
            sceneryConverter_Tests.cpp \
            \#RoadNetworkMapper_Tests.cpp \
            \ # AGENTADAPTER-TESTS:
            agentAdapter_Tests.cpp \
            fakeLaneManager_Tests.cpp \
            worldQuery_Tests.cpp \
            lane_Tests.cpp \
            sensorView_Tests.cpp \
            datatypes_Tests.cpp \
            worldToRoadCoordinateConverter_Tests.cpp \
            egoAgent_Tests.cpp

HEADERS += \
            $$INC_COMMONS \
            $$INC_OWL \
            $$INC_OTHER \
            $$INC_WORLD \
            $$INC_LOCALIZATION \
            AgentManager_Testhelper.h

LIBS += \
    -lopen_simulation_interface \
    -lprotobuf
