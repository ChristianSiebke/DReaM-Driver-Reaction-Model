/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once

#include "InterpreterInterface.h"
#include "Common/WorldRepresentation.h"
#include "qglobal.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometry.hpp>

typedef boost::geometry::model::d2::point_xy<double> point_t;
typedef boost::geometry::model::polygon<point_t> polygon_t;
typedef boost::geometry::model::box<point_t> box_t;

namespace bg = boost::geometry;
namespace bt = boost::geometry::strategy::transform;

typedef bg::model::point<double, 2, bg::cs::cartesian> point_type;
namespace trans = boost::geometry::strategy::transform;

namespace Interpreter {
class CollisionInterpreter : public InterpreterInterface {
  public:
    CollisionInterpreter(LoggerInterface* logger, const BehaviourData& behaviourData) : InterpreterInterface(logger, behaviourData) {}
    CollisionInterpreter(const CollisionInterpreter&) = delete;
    CollisionInterpreter(CollisionInterpreter&&) = delete;
    CollisionInterpreter& operator=(const CollisionInterpreter&) = delete;
    CollisionInterpreter& operator=(CollisionInterpreter&&) = delete;
    virtual ~CollisionInterpreter() override = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) override;

  private:
    void DetermineCollisionPoints(WorldInterpretation* interpretation, const WorldRepresentation& representation);

    std::optional<CollisionPoint> CalculationCollisionPoint(const WorldRepresentation& representation,
                                                            const AgentRepresentation& observedAgent) const;
    std::optional<CollisionPoint> PerformCollisionPointCalculation(double timeStart, double timeEnd,
                                                                   const WorldRepresentation& representation,
                                                                   const AgentRepresentation& observedAgent) const;

    polygon_t ConstructAgentPolygonRepresentation(const AgentRepresentation& data, const Common::Vector2d pos, const double hdg) const;

    polygon_t ConstructPolygonRepresentation(const AgentRepresentation &data, const Common::Vector2d pos, const double hdg) const;
};
} // namespace Interpreter
