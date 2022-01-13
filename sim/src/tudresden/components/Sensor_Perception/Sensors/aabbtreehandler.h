#ifndef AABBTREEHANDLER_H
#define AABBTREEHANDLER_H

#include <unordered_map>

#include "AABBTree/aabbtree.h"
#include "Objects/observeddynamicobject.h"
#include "Objects/observedstaticobject.h"
#include "Objects/observedtrafficsign.h"
#include "core/opSimulation/modules/World_OSI/WorldData.h"

class AABBTreeHandler
{
public:
    static std::shared_ptr<AABBTreeHandler> GetInstance(WorldInterface *world)
    {
        if (!instance)
            instance = std::shared_ptr<AABBTreeHandler>(new AABBTreeHandler(world));
        return instance;
    }
    std::shared_ptr<AABBTree> GetCurrentAABBTree(int timestamp);
    static void ResetAABBTreeHandler()
    {
        instance.reset();
    }

public:
    AABBTreeHandler(const AABBTreeHandler &) = delete;
    AABBTreeHandler &operator=(const AABBTreeHandler &) = delete;
    ~AABBTreeHandler()
    {
    }

public:
    std::unordered_map<AgentInterface *, std::shared_ptr<ObservedDynamicObject>> agentsMapping;

    std::vector<AgentInterface *> agents;
    std::vector<std::shared_ptr<ObservedDynamicObject>> agentObjects;

    std::vector<std::shared_ptr<ObservedStaticObject>> stationaryObjects;
    std::vector<std::shared_ptr<ObservedTrafficSign>> trafficSigns;

    std::unordered_map<std::shared_ptr<ObservedTrafficSign>, OWL::Interfaces::TrafficSign *> trafficSignsMappingReversed;

private:
    AABBTreeHandler(WorldInterface *world);

    void FirstExecution();

    static Common::Vector2d RotatePointAroundPoint(Common::Vector2d input, Common::Vector2d pivot, double angle)
    {
        auto outputX = std::cos(angle) * (input.x - pivot.x) - std::sin(angle) * (input.y - pivot.y) + pivot.x;
        auto outputY = std::sin(angle) * (input.x - pivot.x) + std::cos(angle) * (input.y - pivot.y) + pivot.y;

        return Common::Vector2d(outputX, outputY);
    }

    static Polygon2d ConstructPolygon(Common::Vector2d a, Common::Vector2d b, Common::Vector2d c, Common::Vector2d d)
    {
        Polygon2d polygon{{{a.x, a.y}, {b.x, b.y}, {c.x, c.y}, {d.x, d.y}, {a.x, a.y}}};
        // assert that a valid polygon was generated
        assert(boost::geometry::is_valid(polygon));
        return polygon;
    }

    static Polygon2d ConstructPolygon(const AgentInterface *agent)
    {
        auto referencePoint = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());

        const auto length = agent->GetLength();
        const auto width = agent->GetWidth();
        const auto yaw = agent->GetYaw();

        auto xPart = (length / 2.0) - agent->GetDistanceReferencePointToLeadingEdge();
        auto cog_offset = Common::Vector2d(xPart, 0);
        cog_offset.Rotate(yaw);
        referencePoint.Sub(cog_offset);

        auto p1 = Common::Vector2d(referencePoint.x + length / 2, referencePoint.y + width / 2);
        auto p2 = Common::Vector2d(referencePoint.x + length / 2, referencePoint.y - width / 2);
        auto p3 = Common::Vector2d(referencePoint.x - length / 2, referencePoint.y - width / 2);
        auto p4 = Common::Vector2d(referencePoint.x - length / 2, referencePoint.y + width / 2);

        auto polygon = ConstructPolygon(RotatePointAroundPoint(p1, referencePoint, yaw), RotatePointAroundPoint(p2, referencePoint, yaw),
                                        RotatePointAroundPoint(p3, referencePoint, yaw), RotatePointAroundPoint(p4, referencePoint, yaw));

        return polygon;
    }

    static Polygon2d ConstructPolygon(const OWL::StationaryObject *object)
    {
        auto referencePoint = Common::Vector2d(object->GetReferencePointPosition().x, object->GetReferencePointPosition().y);

        const auto length = object->GetDimension().length;
        const auto width = object->GetDimension().width;
        const auto yaw = object->GetAbsOrientation().yaw;

        auto p1 = Common::Vector2d(referencePoint.x + length / 2, referencePoint.y + width / 2);
        auto p2 = Common::Vector2d(referencePoint.x + length / 2, referencePoint.y - width / 2);
        auto p3 = Common::Vector2d(referencePoint.x - length / 2, referencePoint.y - width / 2);
        auto p4 = Common::Vector2d(referencePoint.x - length / 2, referencePoint.y + width / 2);

        auto polygon = ConstructPolygon(RotatePointAroundPoint(p1, referencePoint, yaw), RotatePointAroundPoint(p2, referencePoint, yaw),
                                        RotatePointAroundPoint(p3, referencePoint, yaw), RotatePointAroundPoint(p4, referencePoint, yaw));

        return polygon;
    }

    static Polygon2d ConstructPolygon(const OWL::Interfaces::TrafficSign *trafficSign)
    {
        auto referencePoint = Common::Vector2d(trafficSign->GetReferencePointPosition().x, trafficSign->GetReferencePointPosition().y);

        const auto length = 0.6;
        const auto width = 0.6;

        auto p1 = Common::Vector2d(referencePoint.x + length / 2, referencePoint.y + width / 2);
        auto p2 = Common::Vector2d(referencePoint.x + length / 2, referencePoint.y - width / 2);
        auto p3 = Common::Vector2d(referencePoint.x - length / 2, referencePoint.y - width / 2);
        auto p4 = Common::Vector2d(referencePoint.x - length / 2, referencePoint.y + width / 2);

        auto polygon = ConstructPolygon(p1, p2, p3, p4);

        return polygon;
    }

private:
    static std::shared_ptr<AABBTreeHandler> instance;
    WorldInterface *world;
    int currentTimestamp = -__INT_MAX__;
    bool firstExecution = true;
    std::shared_ptr<AABBTree> aabbTree;
};

#endif // AABBTREEHANDLER_H
