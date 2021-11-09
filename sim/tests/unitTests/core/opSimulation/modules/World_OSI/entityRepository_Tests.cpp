/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "EntityRepository.h"
#include "fakeDataBuffer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::NiceMock;

using namespace openpass::entity;
using namespace openpass::type;

/**
 * @brief Wraps generation of the Repository
 *
 * @note Internal repository also accessable via "->" operator
 */
struct REPOSITORY_TEST_WRAPPER
{
    NiceMock<FakeDataBuffer> databuffer;
    Repository repository{Repository(&databuffer)};

    Repository *operator->()
    {
        return &repository;
    }
};

TEST(EntitityRepository, RegisterFirstMovingObject_Returns0)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::MovingObject, EntityInfo());
    ASSERT_THAT(entityId, 0);
}

TEST(EntitityRepository, RegisterNextMovingObject_Returns1)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::MovingObject, EntityInfo());
    auto nextEntityId = repo->Register(EntityType::MovingObject, EntityInfo());
    ASSERT_THAT(nextEntityId, 1);
}

TEST(EntitityRepository, RegisterFirstStationaryObject_Returns1000000)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::StationaryObject, EntityInfo());
    ASSERT_THAT(entityId, 1000000);
}

TEST(EntitityRepository, RegisterNextStationaryObject_Returns1000001)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::StationaryObject, EntityInfo());
    auto nextEntityId = repo->Register(EntityType::StationaryObject, EntityInfo());
    ASSERT_THAT(nextEntityId, 1000001);
}

TEST(EntitityRepository, RegisterAnyObject_Returns2000000)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityInfo());
    ASSERT_THAT(entityId, 2000000);
}

TEST(EntitityRepository, RegisteredObject_IsAddedToDatabuffer)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    EXPECT_CALL(repo.databuffer, PutStatic(_, _, _));
    auto entityId = repo->Register(EntityInfo());
}

TEST(EntitityRepository, ResetForMovingObjects_RestartsIndices)
{
    auto repo = REPOSITORY_TEST_WRAPPER();

    auto entityIdObj1 = repo->Register(EntityType::MovingObject, EntityInfo());
    repo->Reset();
    auto entityIdObj2 = repo->Register(EntityType::MovingObject, EntityInfo());

    ASSERT_THAT(entityIdObj1, entityIdObj2);
}

TEST(EntitityRepository, ResetForStaticObjects_DoesNotRestartIndices)
{
    auto repo = REPOSITORY_TEST_WRAPPER();

    auto entityIdObj1 = repo->Register(EntityType::StationaryObject, EntityInfo());
    repo->Reset();
    auto entityIdObj2 = repo->Register(EntityType::StationaryObject, EntityInfo());

    ASSERT_THAT(entityIdObj1, Ne(entityIdObj2));
}
