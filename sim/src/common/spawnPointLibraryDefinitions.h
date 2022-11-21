/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class ParameterInterface;
class ScenarioInterface;
class WorldInterface;
class StochasticsInterface;
class AgentBlueprintProviderInterface;
namespace core
{
    class AgentFactoryInterface;
}

enum class SpawnPointType
{
    PreRun = 0,
    Runtime
};

static const std::unordered_map<SpawnPointType, std::string> spawnPointTypeMapping
{
    {SpawnPointType::PreRun, "PreRun"},
    {SpawnPointType::Runtime, "Runtime"}
};

struct SpawnPointLibraryInfo
{
    std::string libraryName {};
    SpawnPointType type {};
    int priority {};
    std::optional<std::string> profileName {};
};
using SpawnPointLibraryInfoCollection = std::vector<SpawnPointLibraryInfo>;

struct SpawnPointDependencies
{
    SpawnPointDependencies() = default;
    SpawnPointDependencies(const SpawnPointDependencies&) = default;
    SpawnPointDependencies(SpawnPointDependencies&&) = default;


    SpawnPointDependencies(core::AgentFactoryInterface* agentFactory,
                           WorldInterface* world,
                           const AgentBlueprintProviderInterface* agentBlueprintProvider,
                           StochasticsInterface* stochastics) :
        agentFactory{agentFactory},
        world{world},
        agentBlueprintProvider{agentBlueprintProvider},
        stochastics{stochastics}
    {}

    SpawnPointDependencies& operator=(const SpawnPointDependencies&) = default;
    SpawnPointDependencies& operator=(SpawnPointDependencies&&) = default;

    core::AgentFactoryInterface* agentFactory {nullptr};
    WorldInterface* world {nullptr};
    const AgentBlueprintProviderInterface* agentBlueprintProvider {nullptr};
    StochasticsInterface* stochastics {nullptr};
    std::optional<ScenarioInterface*> scenario{std::nullopt};
    std::optional<ParameterInterface*> parameters{std::nullopt};
};
