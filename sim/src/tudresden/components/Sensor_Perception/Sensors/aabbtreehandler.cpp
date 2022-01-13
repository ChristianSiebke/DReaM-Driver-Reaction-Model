#include "aabbtreehandler.h"

#include <string>

std::shared_ptr<AABBTreeHandler> AABBTreeHandler::instance = nullptr;

AABBTreeHandler::AABBTreeHandler(WorldInterface *world) :
    world(world)
{
}

void AABBTreeHandler::FirstExecution()
{
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto stationaryObjects = worldData->GetStationaryObjects();
    auto trafficSigns = worldData->GetTrafficSigns();
    auto agents = world->GetAgents();

    // initial tree generated (size = size of all objects in the world)
    aabbTree = std::make_shared<AABBTree>(stationaryObjects.size() + trafficSigns.size() + agents.size());

    for (const auto &[_, stationaryObject] : stationaryObjects)
    {
        auto obj = std::make_shared<ObservedStaticObject>();
        obj->area = ConstructPolygon(stationaryObject);
        obj->RecalculateAABB();
        obj->objectType = ObservedObjectType::Building;
        obj->id = stationaryObject->GetId();

        aabbTree->InsertObject(obj);
        this->stationaryObjects.push_back(obj);
    }

    for (const auto &[_, trafficSign] : trafficSigns)
    {
        auto obj = std::make_shared<ObservedTrafficSign>();
        obj->area = ConstructPolygon(trafficSign);
        obj->RecalculateAABB();
        obj->objectType = ObservedObjectType::TrafficSign;
        obj->referencePosition = Common::Vector2d(trafficSign->GetReferencePointPosition().x, trafficSign->GetReferencePointPosition().y);
        try
        {
            obj->id = std::stoi(trafficSign->GetId());
        }
        catch (std::invalid_argument const &ex)
        {
            printf("Error while trying to add traffic sign with id %s to the AABBTree, ignoring.\nPlease be aware that only integer ids are supported.", obj->id);
            continue;
        }

        aabbTree->InsertObject(obj);
        this->trafficSigns.push_back(obj);
        this->trafficSignsMappingReversed.insert(std::make_pair(obj, trafficSign));
    }

    for (const auto &[_, agent] : agents)
    {
        auto obj = std::make_shared<ObservedDynamicObject>();
        obj->area = ConstructPolygon(agent);
        obj->RecalculateAABB();
        obj->objectType = ObservedObjectType::MovingObject;
        obj->id = agent->GetId();

        aabbTree->InsertObject(obj);
        this->agentsMapping.insert(std::make_pair(agent, obj));
        this->agents.push_back(agent);
        this->agentObjects.push_back(obj);
        // this->agentsInversed.insert(std::make_pair(obj, agent));
    }
}

std::shared_ptr<AABBTree> AABBTreeHandler::GetCurrentAABBTree(int timestamp)
{
    if (firstExecution)
    {
        FirstExecution();
        firstExecution = false;
    }

    // ensure that the AABBTree is only updated once for every timestamp
    if (timestamp <= currentTimestamp)
        return aabbTree;

    auto worldAgents = world->GetAgents();

    for (const auto &[_, agent] : worldAgents)
    {
        if (agentsMapping.find(agent) != agentsMapping.end())
        {
            auto toUpdate = this->agentsMapping[agent];
            toUpdate->area = ConstructPolygon(agent);
            toUpdate->RecalculateAABB();
            aabbTree->UpdateObject(toUpdate);
        }
        else
        {
            // if new agents are spawned
            auto obj = std::make_shared<ObservedDynamicObject>();
            obj->area = ConstructPolygon(agent);
            obj->RecalculateAABB();
            obj->objectType = ObservedObjectType::MovingObject;
            obj->id = agent->GetId();

            aabbTree->InsertObject(obj);
            this->agentsMapping.insert(std::make_pair(agent, obj));
            this->agents.push_back(agent);
            this->agentObjects.push_back(obj);
        }
    }
    // delete agents if they leave road network
    for (auto iter = agentsMapping.begin(); iter != agentsMapping.end();)
    {
        if (std::none_of(worldAgents.begin(), worldAgents.end(),
                         [iter](std::pair<int, AgentInterface *> element) { return element.second == iter->first; }))
        {
            agents.erase(std::remove_if(agents.begin(), agents.end(), [iter](AgentInterface *element) { return element == iter->first; }),
                         agents.end());
            auto agentObjectsIter =
                std::find_if(agentObjects.begin(), agentObjects.end(),
                             [iter](const std::shared_ptr<ObservedDynamicObject> &element) { return element->id == iter->first->GetId(); });
            agentObjects.erase(agentObjectsIter);
            aabbTree->RemoveObject(*agentObjectsIter);
            iter = agentsMapping.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    currentTimestamp = timestamp;
    return aabbTree;
}
