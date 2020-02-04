/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

#include "OWL/DataTypes.h"

#include "fakeLane.h"
#include "fakeSection.h"
#include "fakeRoad.h"
#include "fakeMovingObject.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::AllOf;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;

static OWL::Id idIterator{0};

static OWL::Id GetNewId()
{
    return ++idIterator;
}

struct FakeLaneManager
{
    const size_t cols;
    const size_t rows;
    const std::vector<double> sectionLengths;

    std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads;
    std::vector<std::vector<OWL::Fakes::Lane*>> lanes;
    std::map<OWL::Id, OWL::Interfaces::Section*> sections;
    std::list<const OWL::Interfaces::Section*> sectionsList;
    std::map<size_t, std::list<const OWL::Interfaces::Lane*>> sectionLanes;
    OWL::Implementation::InvalidLane invalidLane;
    std::vector<std::vector<OWL::Interfaces::WorldObjects*>> objectAssignments;
    std::unordered_map<OWL::Id, OWL::OdId> laneIdMapping;
    std::unordered_map<OWL::Id, std::string> roadIdMapping;

    void SetLength(size_t col, size_t row, double length, double startDistance = 0)
    {
        ON_CALL(*lanes[col][row], GetLength())
        .WillByDefault(Return(length));
        ON_CALL(*lanes[col][row], GetDistance(OWL::MeasurementPoint::RoadStart))
        .WillByDefault(Return(startDistance));
        ON_CALL(*lanes[col][row], GetDistance(OWL::MeasurementPoint::RoadEnd))
        .WillByDefault(Return(startDistance + length));
    }

    void SetWidth(size_t col, size_t row, double width, double distance)
    {
        ON_CALL(*lanes[col][row], GetWidth(distance)).WillByDefault(Return(width));
    }

    void AddWorldObject(size_t col, size_t row, OWL::Interfaces::WorldObject& worldObject)
    {
        objectAssignments[col][row]->push_back(&worldObject);
    }

    void GenerateLaneMatrix(OWL::Interfaces::Road* road)
    {
        OWL::Id streamId[rows];
        for (size_t col = 0; col < cols; col++)
        {
            std::vector<OWL::Fakes::Lane*> fakeLaneRow;
            std::vector<OWL::Interfaces::WorldObjects*> objectAssignmentRow;

            for (size_t row = 0; row < rows; row++)
            {
                OWL::Fakes::Lane* newLane = new OWL::Fakes::Lane();
                OWL::Id id = GetNewId();
                if (col == 0) { streamId[row] = id; }
                ON_CALL(*newLane, GetId()).WillByDefault(Return(id));
                ON_CALL(*newLane, GetLaneType()).WillByDefault(Return(LaneType::Driving));
                ON_CALL(*newLane, GetRoad()).WillByDefault(ReturnRef(*road));
                laneIdMapping.insert({id, -(1 + row)});
                fakeLaneRow.push_back(newLane);
                objectAssignmentRow.push_back(new OWL::Interfaces::WorldObjects());
            }
            lanes.push_back(fakeLaneRow);
            objectAssignments.push_back(objectAssignmentRow);
        }
    }

    void GenerateRoad(double width, std::string roadId = "TestRoadId")
    {
        OWL::Id id = GetNewId();
        roadIdMapping.insert({id, roadId});
        OWL::Fakes::Road* road = new OWL::Fakes::Road();
        GenerateLaneMatrix(road);
        ConnectLaneRows();
        ConnectLaneColumns();
        SetDefaultWidth(width);
        SetDefaultLength(0);
        SetDefaultMovingObjects();
        GenerateSections();
        ON_CALL(*road, GetSections()).WillByDefault(ReturnRef(sectionsList));
        ON_CALL(*road, GetId()).WillByDefault(Return(id));
        roads.insert({id, road});
    }

    void GenerateSections()
    {
        double startOfSection = 0;
        double endOfSection = 0;

        for (size_t col = 0; col < cols; col++)
        {
            OWL::Fakes::Section* section = new OWL::Fakes::Section();
            for (size_t row = 0; row < rows; row++)
            {
                sectionLanes[col].push_back(lanes[col][row]);
                SetLength(col, row, sectionLengths[col], startOfSection);
                ON_CALL(*lanes[col][row], GetSection()).WillByDefault(ReturnRef(*section));
            }

            ON_CALL(*section, GetLanes()).WillByDefault(ReturnRef(sectionLanes[col]));
            assert(col < sectionLengths.size());
            endOfSection = startOfSection + sectionLengths[col];

            ON_CALL(*section, Covers(_)).WillByDefault(Return(false));
            ON_CALL(*section, Covers(AllOf(Ge(startOfSection), Lt(endOfSection)))).WillByDefault(Return(true));

            ON_CALL(*section, CoversInterval(_, _)).WillByDefault(Return(false));
            ON_CALL(*section, CoversInterval(AllOf(Ge(startOfSection), Le(endOfSection)), AllOf(Ge(startOfSection),
                                             Le(endOfSection)))).WillByDefault(Return(true));
            ON_CALL(*section, CoversInterval(AllOf(Ge(startOfSection), Le(endOfSection)), _)).WillByDefault(Return(true));
            ON_CALL(*section, CoversInterval(_, AllOf(Ge(startOfSection), Le(endOfSection)))).WillByDefault(Return(true));
            if (col == cols - 1)
            {
                ON_CALL(*section, Covers(endOfSection)).WillByDefault(Return(true));
            }

            sections.insert({GetNewId(), section});
            sectionsList.push_back(section);
            startOfSection = endOfSection;
        }
    }

    void ConnectLaneColumns()
    {
        for (size_t col = 0; col < cols; col++)
        {
            for (size_t row = 0; row < rows; row++)
            {
                if (col < cols - 1)
                {
                    ON_CALL(*lanes[col][row], GetNext()).WillByDefault(Return(std::vector<OWL::Id>{lanes[col + 1][row]->GetId()}));
                }

                if (col > 0)
                {
                    ON_CALL(*lanes[col][row], GetPrevious()).WillByDefault(Return(std::vector<OWL::Id>{lanes[col - 1][row]->GetId()}));
                }
            }
        }
    }

    void ConnectLaneRows()
    {
        for (size_t col = 0; col < cols; col++)
        {
            for (size_t row = 0; row < rows; row++)
            {
                ON_CALL(*lanes[col][row], Exists()).WillByDefault(Return(true));

                if (row < rows - 1)
                {
                    ON_CALL(*lanes[col][row], GetRightLane()).WillByDefault(ReturnRef(*lanes[col][row + 1]));
                }
                else
                {
                    ON_CALL(*lanes[col][row], GetRightLane()).WillByDefault(ReturnRef(invalidLane));
                }

                if (row > 0)
                {
                    ON_CALL(*lanes[col][row], GetLeftLane()).WillByDefault(ReturnRef(*lanes[col][row - 1]));
                }
                else
                {
                    ON_CALL(*lanes[col][row], GetLeftLane()).WillByDefault(ReturnRef(invalidLane));
                }
            }
        }
    }

    void SetDefaultWidth(double width)
    {
        for (size_t col = 0; col < cols; col++)
        {
            for (size_t row = 0; row < rows; row++)
            {
                ON_CALL(*lanes[col][row], GetWidth(_)).WillByDefault(Return(width));
            }
        }
    }

    void SetDefaultLength(double length)
    {
        for (size_t col = 0; col < cols; col++)
        {
            for (size_t row = 0; row < rows; row++)
            {
                SetLength(col, row, length);
            }
        }
    }

    void SetDefaultMovingObjects()
    {
        for (size_t col = 0; col < cols; col++)
        {
            for (size_t row = 0; row < rows; row++)
            {
                ON_CALL(*lanes[col][row], GetWorldObjects()).WillByDefault(ReturnRef(*objectAssignments[col][row]));
            }
        }
    }

    OWL::Fakes::Lane& GetLane(size_t cols, size_t rows)
    {
        return *lanes[cols][rows];
    }

    const std::map<OWL::Id, OWL::Interfaces::Section* >& GetSections() const
    {
        return sections;
    }

    const std::unordered_map<OWL::Id, OWL::Interfaces::Road* >& GetRoads() const
    {
        return roads;
    }

    std::unordered_map<OWL::Id, OWL::OdId>& GetLaneIdMapping()
    {
        return laneIdMapping;
    }

    std::unordered_map<OWL::Id, std::string>& GetRoadIdMapping()
    {
        return roadIdMapping;
    }

    FakeLaneManager(size_t cols, size_t rows, double width, const std::vector<double>& sectionLengths, const std::string& roadId) : cols{cols}, rows{rows},
        sectionLengths{sectionLengths}
    {
        assert(cols > 0);
        assert(rows > 0);

        GenerateRoad(width, roadId);
    }

    ~FakeLaneManager()
    {
        for (auto fakeLaneRow : lanes)
        {
            for (auto fakeLane : fakeLaneRow) { delete fakeLane; }
        }

        for (auto objectAssignmentRow : objectAssignments)
        {
            for (auto objectAssignment : objectAssignmentRow) { delete objectAssignment; }
        }

        for (auto section : sections) { delete section.second; }

        for (auto road : roads) { delete road.second; }
    }
};
