/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "fakeDataStore.h"
#include "EntityRepository.h"

using ::testing::NiceMock;
using ::testing::Eq;
using ::testing::_;

using namespace openpass::entity;

/**
 * @brief Wraps generation of the Repository
 *
 * @note Internal repository also accessable via "->" operator
 */
struct REPOSITORY_TEST_WRAPPER
{
    NiceMock<FakeDataStore> datastore;
    Repository repository { Repository(&datastore) };

    Repository *operator->()
    {
        return &repository;
    }
};

EntityMetaInfo TEST_META_INFO()
{
    return EntityMetaInfo("dummy");
}

TEST(EntitityRepository, RegisterFirstMovingObject_Returns0)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::MovingObject, TEST_META_INFO());
    ASSERT_THAT(entityId, 0);
}

TEST(EntitityRepository, RegisterNextMovingObject_Returns1)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::MovingObject, TEST_META_INFO());
    auto nextEntityId = repo->Register(EntityType::MovingObject, TEST_META_INFO());
    ASSERT_THAT(nextEntityId, 1);
}

TEST(EntitityRepository, RegisterFirstStationaryObject_Returns100000)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::StationaryObject, TEST_META_INFO());
    ASSERT_THAT(entityId, 100000);
}

TEST(EntitityRepository, RegisterNextStationaryObject_Returns100001)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(EntityType::StationaryObject, TEST_META_INFO());
    auto nextEntityId = repo->Register(EntityType::StationaryObject, TEST_META_INFO());
    ASSERT_THAT(nextEntityId, 100001);
}

TEST(EntitityRepository, RegisterAnyObject_Returns200000)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityId = repo->Register(TEST_META_INFO());
    ASSERT_THAT(entityId, 200000);
}

TEST(EntitityRepository, RegisteredObject_IsAddedToDatastore)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    EXPECT_CALL(repo.datastore, PutAcyclic(_, _, _, _));
    auto entityId = repo->Register(TEST_META_INFO());
}

TEST(EntitityRepository, Reset_RestartsIndices)
{
    auto repo = REPOSITORY_TEST_WRAPPER();
    auto entityIdObj1_1 = repo->Register(EntityType::MovingObject, TEST_META_INFO());
    auto entityIdObj1_2 = repo->Register(EntityType::StationaryObject, TEST_META_INFO());

    repo->Reset();

    auto entityIdObj2_1 = repo->Register(EntityType::MovingObject, TEST_META_INFO());
    auto entityIdObj2_2 = repo->Register(EntityType::StationaryObject, TEST_META_INFO());

    ASSERT_THAT(entityIdObj1_1, entityIdObj2_1);
    ASSERT_THAT(entityIdObj1_2, entityIdObj2_2);
}