/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cmath>

#include "fakeMovingObject.h"
#include "fakeTrafficSign.h"
#include "Primitives.h"
#include "WorldData.h"

#include "osi/osi_sensorview.pb.h"
#include "osi/osi_sensorviewconfiguration.pb.h"

using namespace OWL;

using ::testing::AllOf;
using ::testing::UnorderedElementsAreArray;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Return;

const OWL::Primitive::AbsPosition frontLeftPosition  {  50.0,  10.0, 0.0 };
const OWL::Primitive::AbsPosition frontPosition      {  50.0,   0.0, 0.0 };
const OWL::Primitive::AbsPosition frontRightPosition {  50.0, -10.0, 0.0 };
const OWL::Primitive::AbsPosition leftPosition       {   0.0,  10.0, 0.0 };
const OWL::Primitive::AbsPosition centerPosition     {   0.0,   0.0, 0.0 };
const OWL::Primitive::AbsPosition rightPosition      {   0.0, -10.0, 0.0 };
const OWL::Primitive::AbsPosition rearLeftPosition   { -50.0,  10.0, 0.0 };
const OWL::Primitive::AbsPosition rearPosition       { -50.0,   0.0, 0.0 };
const OWL::Primitive::AbsPosition rearRightPosition  { -50.0, -10.0, 0.0 };

template<typename K, typename V>
int CountMapValues(const std::unordered_map<K, V>& theMap, const V value)
{
    return std::count_if(theMap.cbegin(),
                         theMap.cend(),
                         [value](const std::pair<K,V>& item)
                         {
                             return item.second == value;
                         });
}

std::vector<int> GetVisibleObjectIds(const std::unordered_map<int, bool>& idVisibility)
{
    std::vector<int> visibleIds;

    for (const auto& mapItem : idVisibility)
    {
        if (mapItem.second)
        {
            visibleIds.push_back(mapItem.first);
        }
    }

    return visibleIds;
}

template <typename T>
std::vector<int> GetObjectIds(const std::vector<T>& objects)
{
    std::vector<int> ids;

    for (const auto& object : objects)
    {
        ids.push_back(object->GetId());
    }

    return ids;
}

std::string ElementsToString(std::vector<int>& vec)
{
    std::string str{""};

    for (auto it = vec.cbegin(); it != vec.cend(); ++it)
    {
        str += std::to_string(*it);

        if (std::next(it) != vec.cend())
        {
            str += ", ";
        }
    }

    return str;
}

class NormalizeAngle_Data
{
public:
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    double angle;
    double normalizedAngle;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const NormalizeAngle_Data& data)
    {
        return os << "angle: " << data.angle << ", normalizedAngle: " << data.normalizedAngle;
    }
};

class ApplySectorFilter_Data
{
public:
    // do not change order of items
    // unless you also change it in INSTANTIATE_TEST_CASE_P
    OWL::Primitive::AbsPosition origin;
    double radius;
    double yawMax;
    double yawMin;
    std::unordered_map<int, bool> idVisibility;

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const ApplySectorFilter_Data& obj)
    {
        os << "origin: (" << obj.origin.x << ", " << obj.origin.y << ", " << obj.origin.z << "), "
           << "radius: " << obj.radius << ", "
           << "yawMax: " << obj.yawMax << ", "
           << "yawMin: " << obj.yawMin << ", "
           << "idVisibility: (" << std::boolalpha;

        for (const auto& id : obj.idVisibility)
        {
            os << id.first << ": " << id.second << ", ";
        }

        os << ")" << std::noboolalpha;

        return os;
    }
};

class NormalizeAngle : public ::testing::TestWithParam<NormalizeAngle_Data>
{
protected:
    OWL::WorldData worldData;
};


class ApplySectorFilter : public ::testing::TestWithParam<ApplySectorFilter_Data>
{
private:
    Fakes::MovingObject fakeMovingFrontLeft;
    Fakes::MovingObject fakeMovingFront;
    Fakes::MovingObject fakeMovingFrontRight;
    Fakes::MovingObject fakeMovingLeft;
    Fakes::MovingObject fakeMovingCenter;
    Fakes::MovingObject fakeMovingRight;
    Fakes::MovingObject fakeMovingRearLeft;
    Fakes::MovingObject fakeMovingRear;
    Fakes::MovingObject fakeMovingRearRight;

protected:
    OWL::WorldData worldData;

    const std::vector<Fakes::MovingObject*> fakeMovingObjects
        { &fakeMovingFrontLeft,
          &fakeMovingFront,
          &fakeMovingFrontRight,
          &fakeMovingLeft,
          &fakeMovingCenter,
          &fakeMovingRight,
          &fakeMovingRearLeft,
          &fakeMovingRear,
          &fakeMovingRearRight };

public:
    ApplySectorFilter()
    {
        ON_CALL(fakeMovingFrontLeft,  GetId()).WillByDefault(Return(1));
        ON_CALL(fakeMovingFront,      GetId()).WillByDefault(Return(2));
        ON_CALL(fakeMovingFrontRight, GetId()).WillByDefault(Return(3));
        ON_CALL(fakeMovingLeft,       GetId()).WillByDefault(Return(4));
        ON_CALL(fakeMovingCenter,     GetId()).WillByDefault(Return(5));
        ON_CALL(fakeMovingRight,      GetId()).WillByDefault(Return(6));
        ON_CALL(fakeMovingRearLeft,   GetId()).WillByDefault(Return(7));
        ON_CALL(fakeMovingRear,       GetId()).WillByDefault(Return(8));
        ON_CALL(fakeMovingRearRight,  GetId()).WillByDefault(Return(9));

        ON_CALL(fakeMovingFrontLeft,  GetReferencePointPosition()).WillByDefault(Return(frontLeftPosition));
        ON_CALL(fakeMovingFront,      GetReferencePointPosition()).WillByDefault(Return(frontPosition));
        ON_CALL(fakeMovingFrontRight, GetReferencePointPosition()).WillByDefault(Return(frontRightPosition));
        ON_CALL(fakeMovingLeft,       GetReferencePointPosition()).WillByDefault(Return(leftPosition));
        ON_CALL(fakeMovingCenter,     GetReferencePointPosition()).WillByDefault(Return(centerPosition));
        ON_CALL(fakeMovingRight,      GetReferencePointPosition()).WillByDefault(Return(rightPosition));
        ON_CALL(fakeMovingRearLeft,   GetReferencePointPosition()).WillByDefault(Return(rearLeftPosition));
        ON_CALL(fakeMovingRear,       GetReferencePointPosition()).WillByDefault(Return(rearPosition));
        ON_CALL(fakeMovingRearRight,  GetReferencePointPosition()).WillByDefault(Return(rearRightPosition));
    }
};


TEST_P(NormalizeAngle, ReturnsAngleWithinPlusMinusPi)
{
    auto data = GetParam();

    double normalizedAngle = worldData.NormalizeAngle(data.angle);

    ASSERT_THAT(normalizedAngle, AllOf(Ge(-M_PI), Le(M_PI)));
    ASSERT_THAT(normalizedAngle, Eq(data.normalizedAngle));
}

INSTANTIATE_TEST_CASE_P(AngleList, NormalizeAngle,
  /*                             angle   expectedNormalizedAngle  */
  testing::Values(
    NormalizeAngle_Data{                  0.0,               0.0 },
    NormalizeAngle_Data{               M_PI_4,            M_PI_4 },
    NormalizeAngle_Data{              -M_PI_4,           -M_PI_4 },
    NormalizeAngle_Data{               M_PI_2,            M_PI_2 },
    NormalizeAngle_Data{                -M_PI,             -M_PI },
    NormalizeAngle_Data{                 M_PI,              M_PI },
    NormalizeAngle_Data{              -M_PI_2,           -M_PI_2 },
    NormalizeAngle_Data{         3.0 * M_PI_4,      3.0 * M_PI_4 },
    NormalizeAngle_Data{        -3.0 * M_PI_4,     -3.0 * M_PI_4 },
    NormalizeAngle_Data{  2.0 * M_PI + M_PI_4,            M_PI_4 },
    NormalizeAngle_Data{ -2.0 * M_PI - M_PI_4,           -M_PI_4 },
    NormalizeAngle_Data{  2.0 * M_PI + M_PI_2,            M_PI_2 },
    NormalizeAngle_Data{ -2.0 * M_PI - M_PI_2,           -M_PI_2 },
    NormalizeAngle_Data{  4.0 * M_PI + M_PI_2,            M_PI_2 },
    NormalizeAngle_Data{ -4.0 * M_PI - M_PI_2,           -M_PI_2 }
  )
);


TEST_P(ApplySectorFilter, ReturnsVisibleObjects)
{
    auto data = GetParam();

    int numberOfVisibleObjects = CountMapValues(data.idVisibility, true);
    std::vector<int> listOfVisibleObjects = GetVisibleObjectIds(data.idVisibility);

    auto filteredMoving = worldData.ApplySectorFilter(fakeMovingObjects, data.origin, data.radius, data.yawMax, data.yawMin);
    auto filteredIds = GetObjectIds(filteredMoving);

    ASSERT_THAT(filteredMoving.size(), Eq(numberOfVisibleObjects)) << "Visible IDs: " << ElementsToString(filteredIds);
    ASSERT_THAT(filteredIds, UnorderedElementsAreArray(listOfVisibleObjects));
}

// angle = 0.0 equals to "east" direction, which shows up "right" in the idVisibility table below
INSTANTIATE_TEST_CASE_P(FullCircle, ApplySectorFilter,
  testing::Values(
    /*                              origin   radius        leftAngle   rightAngle                                    idVisibility */
    ApplySectorFilter_Data{ centerPosition,     5.0,    3.0 * M_PI_2,     -M_PI_2, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    20.0,    3.0 * M_PI_2,     -M_PI_2, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    50.0,    3.0 * M_PI_2,     -M_PI_2, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,    3.0 * M_PI_2,     -M_PI_2, { { 7, true  }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,     5.0,          M_PI,         -M_PI, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    20.0,          M_PI,         -M_PI, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    50.0,          M_PI,         -M_PI, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,          M_PI,         -M_PI, { { 7, true  }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,     5.0,        M_PI_2, -3.0 * M_PI_2, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    20.0,        M_PI_2, -3.0 * M_PI_2, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    50.0,        M_PI_2, -3.0 * M_PI_2, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,        M_PI_2, -3.0 * M_PI_2, { { 7, true  }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,     5.0,    2.0 * M_PI,           0.0, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    20.0,    2.0 * M_PI,           0.0, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    50.0,    2.0 * M_PI,           0.0, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,    2.0 * M_PI,           0.0, { { 7, true  }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{  frontPosition,    25.0,          M_PI,         -M_PI, { { 7, false }, { 4, false }, { 1, true  },
                                                                                     { 8, false }, { 5, false }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, true  } } },

    ApplySectorFilter_Data{   rearPosition,    25.0,          M_PI,         -M_PI, { { 7, true  }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, false }, { 2, false },
                                                                                     { 9, true  }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{   leftPosition,    15.0,          M_PI,         -M_PI, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{  rightPosition,    15.0,          M_PI,         -M_PI, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } }

  )
);

// angle = 0.0 equals to "east" direction, which shows up "right" in the idVisibility table below
INSTANTIATE_TEST_CASE_P(InsideConeOfInterest, ApplySectorFilter,
  testing::Values(
    /*                              origin   radius      leftAngle     rightAngle                                    idVisibility */
    ApplySectorFilter_Data{ centerPosition,    70.0,  3.0 * M_PI_4,        M_PI_4, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  M_PI_2 + 0.1,  M_PI_2 - 0.1, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  3.0 * M_PI_4,  M_PI_2 - 0.1, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,        M_PI_4,       -M_PI_4, { { 7, false }, { 4, false }, { 1, true  },
                                                                                     { 8, false }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,           0.1,          -0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,        M_PI_4,          -0.1, { { 7, false }, { 4, false }, { 1, true  },
                                                                                     { 8, false }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  7.0 * M_PI_4,  5.0 * M_PI_4, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,       -M_PI_4, -3.0 * M_PI_4, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,   3.0 * M_PI_2 + 0.1,  3.0 * M_PI_2 - 0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                                                  { 8, false }, { 5, true  }, { 2, false },
                                                                                                  { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,         7.0 * M_PI_4,  3.0 * M_PI_2 - 0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                                                  { 8, false }, { 5, true  }, { 2, false },
                                                                                                  { 9, false }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  5.0 * M_PI_4,  3.0 * M_PI_4, { { 7, true  }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, true  }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0, -3.0 * M_PI_4, -5.0 * M_PI_4, { { 7, true  }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, true  }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,    M_PI + 0.1,    M_PI - 0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  5.0 * M_PI_4,    M_PI - 0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, true  }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,        M_PI_2,           0.0, { { 7, false }, { 4, true  }, { 1, true  },
                                                                                     { 8, false }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,    2.0 * M_PI,  3.0 * M_PI_2, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,           0.0,       -M_PI_2, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  3.0 * M_PI_2,          M_PI, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, true  }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,       -M_PI_2,          M_PI, { { 7, false }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, true  }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,          M_PI,        M_PI_2, { { 7, true  }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,         -M_PI, -3.0 * M_PI_2, { { 7, true  }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,           0.0,        M_PI_2, { { 7, true  }, { 4, true  }, { 1, false },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,        M_PI_2,          M_PI, { { 7, false }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  3.0 * M_PI_2,    2.0 * M_PI, { { 7, true  }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, true  }, { 6, true  }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,          M_PI,  3.0 * M_PI_2, { { 7, true  }, { 4, true  }, { 1, true  },
                                                                                     { 8, true  }, { 5, true  }, { 2, true  },
                                                                                     { 9, false }, { 6, true  }, { 3, true  } } },

    ApplySectorFilter_Data{ centerPosition,    70.0,  M_PI_2 + 0.1,  M_PI_2 - 0.1, { { 7, false }, { 4, true  }, { 1, false },
                                                                                     { 8, false }, { 5, true  }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{  frontPosition,    70.0,  M_PI_2 + 0.1,  M_PI_2 - 0.1, { { 7, false }, { 4, false }, { 1, true  },
                                                                                     { 8, false }, { 5, false }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{   rearPosition,    70.0,  M_PI_2 + 0.1,  M_PI_2 - 0.1, { { 7, true  }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, false }, { 2, false },
                                                                                     { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{frontLeftPosition,  70.0, -M_PI_2 + 0.1, -M_PI_2 - 0.1, { { 7, false }, { 4, false }, { 1, true  },
                                                                                     { 8, false }, { 5, false }, { 2, true  },
                                                                                     { 9, false }, { 6, false }, { 3, true  } } },

    ApplySectorFilter_Data{ rearLeftPosition,  70.0, -M_PI_2 + 0.1, -M_PI_2 - 0.1, { { 7, true  }, { 4, false }, { 1, false },
                                                                                     { 8, true  }, { 5, false }, { 2, false },
                                                                                     { 9, true  }, { 6, false }, { 3, false } } }

  )
);

INSTANTIATE_TEST_CASE_P(ZeroRadius, ApplySectorFilter,
  testing::Values(
    /*                              origin   orientation   radius   yawMax   yawMin   idVisibility */
    ApplySectorFilter_Data{ centerPosition,   0.0,  M_PI_4, -M_PI_4, { { 7, false }, { 4, false }, { 1, false },
                                                                       { 8, false }, { 5, false }, { 2, false },
                                                                       { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,   0.0,     0.1,    -0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                       { 8, false }, { 5, false }, { 2, false },
                                                                       { 9, false }, { 6, false }, { 3, false } } },

    ApplySectorFilter_Data{ centerPosition,   0.0,  M_PI_4,    -0.1, { { 7, false }, { 4, false }, { 1, false },
                                                                       { 8, false }, { 5, false }, { 2, false },
                                                                       { 9, false }, { 6, false }, { 3, false } } }
  )
);
