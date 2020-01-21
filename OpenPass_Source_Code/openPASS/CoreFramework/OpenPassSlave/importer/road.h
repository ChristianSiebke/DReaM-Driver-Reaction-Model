/*******************************************************************************
 * Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
 *               2016, 2017, 2018 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Road.h
//! @brief This file contains the internal representation of a road in a
//!        scenery.
//-----------------------------------------------------------------------------

#pragma once

#include "Common/vector2d.h"
#include "CoreFramework/CoreShare/log.h"
#include "Interfaces/roadInterface/roadInterface.h"
#include "Interfaces/worldInterface.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <list>
#include <map>

#include "road/roadObject.h"
#include "road/roadSignal.h"

class Road;
class RoadLane;
class RoadLaneSection;

//-----------------------------------------------------------------------------
//! Class representing a link to a road.
//-----------------------------------------------------------------------------
class RoadLink : public RoadLinkInterface
{
  public:
    RoadLink(RoadLinkType type, RoadLinkElementType elementType, const std::string &elementId,
             ContactPointType contactPoint, RoadLinkDirectionType direction, RoadLinkSideType side)
        : type(type), elementType(elementType), elementId(elementId), contactPoint(contactPoint), direction(direction),
          side(side)
    {
    }
    virtual ~RoadLink() override = default;

    //-----------------------------------------------------------------------------
    //! Returns the RoadLinkType of the RoadLink.
    //!
    //! @return                         RoadLinkType of the RoadLink
    //-----------------------------------------------------------------------------
    RoadLinkType GetType() const override
    {
        return type;
    }

    //-----------------------------------------------------------------------------
    //! Returns the RoadLinkElementType of the RoadLink.
    //!
    //! @return                         RoadLinkElementType of the RoadLink
    //-----------------------------------------------------------------------------
    RoadLinkElementType GetElementType() const override
    {
        return elementType;
    }

    //-----------------------------------------------------------------------------
    //! Returns the ID of the RoadLink.
    //!
    //! @return                         ID of the RoadLink
    //-----------------------------------------------------------------------------
    const std::string &GetElementId() const override
    {
        return elementId;
    }

    //-----------------------------------------------------------------------------
    //! Returns the ContactPointType of the RoadLink.
    //!
    //! @return                         ContactPointType of the RoadLink
    //-----------------------------------------------------------------------------
    ContactPointType GetContactPoint() const override
    {
        return contactPoint;
    }

    //-----------------------------------------------------------------------------
    //! Returns the RoadLinkDirectionType of the RoadLink.
    //!
    //! @return                         RoadLinkDirectionType of the RoadLink
    //-----------------------------------------------------------------------------
    RoadLinkDirectionType GetDirection() const override
    {
        return direction;
    }

    //-----------------------------------------------------------------------------
    //! Returns the RoadLinkSideType of the RoadLink.
    //!
    //! @return                         RoadLinkSideType of the RoadLink
    //-----------------------------------------------------------------------------
    RoadLinkSideType GetSide() const override
    {
        return side;
    }

  private:
    RoadLinkType type;
    RoadLinkElementType elementType;
    std::string elementId;
    ContactPointType contactPoint;
    RoadLinkDirectionType direction;
    RoadLinkSideType side;
};

//-----------------------------------------------------------------------------
//! Class representing a road lane.
//-----------------------------------------------------------------------------
class RoadLane : public RoadLaneInterface
{
  public:
    RoadLane(RoadLaneSectionInterface *laneSection, int id, RoadLaneType type)
        : laneSection(laneSection), id(id), type(type)
    {
    }
    virtual ~RoadLane() override;

    //-----------------------------------------------------------------------------
    //! Adds a new polynomial calculating the width of a lane to a road lane.
    //!
    //! @param[in]  sOffset             Offset relative to the preceding lane section
    //! @param[in]  a                   Constant factor from the polynomial
    //! @param[in]  b                   Linear factor from the polynomial
    //! @param[in]  c                   Quadratic factor from the polynomial
    //! @param[in]  d                   Cubic factor from the polynomial
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddWidth(double sOffset, double a, double b, double c, double d) override;

    //-----------------------------------------------------------------------------
    //! Adds the ID of a successor lane to a road lane.
    //!
    //! @param[in]  id                  ID of a successor lane
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddSuccessor(int id) override;

    //-----------------------------------------------------------------------------
    //! Adds the ID of a predecessor lane to a road lane.
    //!
    //! @param[in]  id                  ID of a predecessor lane
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddPredecessor(int id) override;

    //-----------------------------------------------------------------------------
    //! Returns the ID of the road lane.
    //!
    //! @return                         ID of the road lane
    //-----------------------------------------------------------------------------
    int GetId() const override
    {
        return id;
    }

    //-----------------------------------------------------------------------------
    //! Returns the RoadLaneTypeType of the road lane.
    //!
    //! @return                         RoadLaneTypeType of the road lane
    //-----------------------------------------------------------------------------
    RoadLaneType GetType() const override
    {
        return type;
    }

    //-----------------------------------------------------------------------------
    //! Returns the widths of the road lane.
    //!
    //! @return                         RoadLaneTypeType of the road lane
    //-----------------------------------------------------------------------------
    const std::list<RoadLaneWidth *> &GetWidths() const override
    {
        return widths;
    }

    //-----------------------------------------------------------------------------
    //! Returns the successors of a road lane, can have at most one element.
    //!
    //! @return                         Successors of the road lane
    //-----------------------------------------------------------------------------
    const std::list<int> &GetSuccessor() const override
    {
        return successor;
    }

    //-----------------------------------------------------------------------------
    //! Returns the predecessors of a road lane, can have at most one element.
    //!
    //! @return                         Predecessors of the road lane
    //-----------------------------------------------------------------------------
    const std::list<int> &GetPredecessor() const override
    {
        return predecessor;
    }

    //-----------------------------------------------------------------------------
    //! Sets the flag, if the road lane is in the reference direction or not.
    //!
    //! @param[in]  inDirection         Flag, if the road lane is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    void SetInDirection(bool inDirection) override
    {
        this->inDirection = inDirection;
    }

    //-----------------------------------------------------------------------------
    //! Returns if the road lane is in the reference direction or not.
    //!
    //! @return                         Flag, if the road lane is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    bool GetInDirection() const override
    {
        return inDirection;
    }

    //-----------------------------------------------------------------------------
    //! Returns the RoadLaneSection to which the road lane belongs.
    //!
    //! @return                         RoadLaneSection to which the road lane belongs
    //-----------------------------------------------------------------------------
    RoadLaneSectionInterface *GetLaneSection() const override
    {
        return laneSection;
    }

    //-----------------------------------------------------------------------------
    //! Adds a road mark to the road
    //!
    //! @param[in]  sOffset             s offset from start of road
    //! @param[in]  type                location type of road mark (left, center, right)
    //! @param[in]  roadMark            Type of the road mark
    //! @param[in]  color               Color of the road mark
    //! @param[in]  laneChange          Allowed lane change directions
    //! @param[in]  weight              Weight of the road mark (standard or bold)
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddRoadMark(double sOffset, RoadLaneRoadDescriptionType type, RoadLaneRoadMarkType roadMark,
                     RoadLaneRoadMarkColor color, RoadLaneRoadMarkLaneChange laneChange,
                     RoadLaneRoadMarkWeight weight) override;

    //-----------------------------------------------------------------------------
    //! Returns the road marks of the road
    //!
    //! @return                         List of road marks
    //-----------------------------------------------------------------------------
    const std::list<RoadLaneRoadMark *> &GetRoadMarks() const override
    {
        return roadMarks;
    }

  private:
    RoadLaneSectionInterface *laneSection;
    int id;
    RoadLaneType type;
    // using lists to indicate empty predecessor/successor
    std::list<RoadLaneWidth *> widths;
    std::list<int> predecessor;
    std::list<int> successor;
    bool inDirection = true;

    RoadLaneRoadMarkType roadMarkType = RoadLaneRoadMarkType::Undefined;
    double roadMarkTypeSOffset;

    std::list<RoadLaneRoadMark *> roadMarks;
};

//-----------------------------------------------------------------------------
//! Class representing a road lane section that can contain several road lanes.
//-----------------------------------------------------------------------------
class RoadLaneSection : public RoadLaneSectionInterface
{
  public:
    RoadLaneSection(RoadInterface *road, double start) : road(road), start(start)
    {
    }
    virtual ~RoadLaneSection() override;

    //-----------------------------------------------------------------------------
    //! Adds and returns a road lane.
    //!
    //! @param[in]  id                  ID of the lane
    //! @param[in]  type                RoadLaneTypeType of the lane
    //!
    //! @return                         False if an error occurred, true otherwise
    //-----------------------------------------------------------------------------
    RoadLane *AddRoadLane(int id, RoadLaneType type) override;

    //-----------------------------------------------------------------------------
    //! Returns the stored road lanes as a mapping from their respective IDs.
    //!
    //! @return                         Stored road lanes as a mapping from their
    //!                                 respective IDs
    //-----------------------------------------------------------------------------
    const std::map<int, RoadLaneInterface *> &GetLanes() const override
    {
        return lanes;
    }

    //-----------------------------------------------------------------------------
    //! Returns the starting offset of the road lane section.
    //!
    //! @return                         Starting offset of the road lane section
    //-----------------------------------------------------------------------------
    double GetStart() const override
    {
        return start;
    }

    //-----------------------------------------------------------------------------
    //! Sets the flag, if the lane section is in the reference direction or not.
    //!
    //! @param[in]  inDirection         Flag, if the road lane is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    void SetInDirection(bool inDirection) override
    {
        this->inDirection = inDirection;
    }

    //-----------------------------------------------------------------------------
    //! Sets the index offset for the road lanes in this section.
    //!
    //! @param[in]  laneIndexOffset     Index offset for the road lanes in this section
    //-----------------------------------------------------------------------------
    void SetLaneIndexOffset(int laneIndexOffset) override
    {
        this->laneIndexOffset = laneIndexOffset;
    }

    //-----------------------------------------------------------------------------
    //! Sets the ID.
    //!
    //! @param[in]  Id             ID
    //-----------------------------------------------------------------------------
    void SetId(int Id) override
    {
        this->Id = Id;
    }

    //-----------------------------------------------------------------------------
    //! Returns if the lane section is in the reference direction or not.
    //!
    //! @return                         Flag, if the lane section is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    bool GetInDirection() const override
    {
        return inDirection;
    }

    //-----------------------------------------------------------------------------
    //! Returns the index offset for the road lanes in this section.
    //!
    //! @return                         Index offset for the road lanes in this section
    //-----------------------------------------------------------------------------
    int GetLaneIndexOffset() const override
    {
        return laneIndexOffset;
    }

    //-----------------------------------------------------------------------------
    //! Returns the ID.
    //!
    //! @return                         ID
    //-----------------------------------------------------------------------------
    int GetId() const override
    {
        return Id;
    }

    //-----------------------------------------------------------------------------
    //! Returns the road from which this section is a part of.
    //!
    //! @return                         Road from which this section is a part of
    //-----------------------------------------------------------------------------
    RoadInterface *GetRoad() const override
    {
        return road;
    }

  private:
    RoadInterface *road;
    const double start;
    std::map<int, RoadLaneInterface *> lanes; // use map for sorted ids
    bool inDirection = true;
    int laneIndexOffset = 0;
    int Id{0};
};

//-----------------------------------------------------------------------------
//! Abstract class representing the geometry of a road, i.e. the layout of the
//! road's reference line in the in the x/y plane.
//-----------------------------------------------------------------------------
class RoadGeometry : public RoadGeometryInterface
{
  public:
    RoadGeometry(double s, double x, double y, double hdg, double length) : s{s}, x{x}, y{y}, hdg{hdg}, length{length}
    {
    }
    virtual ~RoadGeometry() override = default;

    //-----------------------------------------------------------------------------
    //! Calculates the x/y coordinates as vector.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  tOffset    offset to the left
    //! @return                coordinates with the x/y coordinates
    //-----------------------------------------------------------------------------
    virtual Common::Vector2d GetCoord(double sOffset, double tOffset) const override = 0;

    //-----------------------------------------------------------------------------
    //! Calculates the heading.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @return                heading
    //-----------------------------------------------------------------------------
    virtual double GetDir(double sOffset) const override = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the s offset within the OpenDrive road.
    //!
    //! @return s offset
    //-----------------------------------------------------------------------------
    double GetS() const override
    {
        return s;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the initial heading of the geometry section
    //!
    //! @return initial heading of geometry
    //-----------------------------------------------------------------------------
    double GetHdg() const override
    {
        return hdg;
    }

    //-----------------------------------------------------------------------------
    //! Retrieves the length of the geometry section
    //!
    //! @return length of geometry section
    //-----------------------------------------------------------------------------
    double GetLength() const override
    {
        return length;
    }

  protected:
    //-----------------------------------------------------------------------------
    //! Gets the coordinate at the s and t offest if the road is linear.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  tOffset    offset to the left
    //! @return                coordinates with the x/y coordinates
    //-----------------------------------------------------------------------------
    Common::Vector2d GetCoordLine(double sOffset, double tOffset) const;

    //-----------------------------------------------------------------------------
    //! Returns the direction of the line, i.e. the start orientation.
    //!
    //! @param[in]  sOffset      offset within geometry section; unused
    //! @return                         direction
    //-----------------------------------------------------------------------------
    double GetDirLine(double sOffset) const;

    //-----------------------------------------------------------------------------
    //! Gets the coordinate at the s and t offest if the road is an arc.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  tOffset    offset to the left
    //! @param[in]  curvature  curvature of the arc
    //! @return                coordinates with the x/y coordinates
    //-----------------------------------------------------------------------------
    Common::Vector2d GetCoordArc(double sOffset, double tOffset, double curvature) const;

    //-----------------------------------------------------------------------------
    //! Returns the heading of an arc, i.e. the initial heading plus the fraction
    //! of the radius due to the curvature.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  curvature  curvature of the arc
    //! @return                heading of the arc
    //-----------------------------------------------------------------------------
    double GetDirArc(double sOffset, double curvature) const;

    double s;
    double x;
    double y;
    double hdg;
    double length;
};

//-----------------------------------------------------------------------------
//! Class representing a road line as a RoadGeometry.
//-----------------------------------------------------------------------------
class RoadGeometryLine : public RoadGeometry
{
  public:
    RoadGeometryLine(double s, double x, double y, double hdg, double length) : RoadGeometry(s, x, y, hdg, length)
    {
    }
    virtual ~RoadGeometryLine() override = default;
    virtual Common::Vector2d GetCoord(double sOffset, double tOffset) const override;
    virtual double GetDir(double sOffset) const override;
};

//-----------------------------------------------------------------------------
//! Class representing a road arc as a RoadGeometry.
//-----------------------------------------------------------------------------
class RoadGeometryArc : public RoadGeometry
{
  public:
    RoadGeometryArc(double s, double x, double y, double hdg, double length, double curvature)
        : RoadGeometry{s, x, y, hdg, length}, curvature{curvature}
    {
    }
    virtual ~RoadGeometryArc() override = default;
    virtual Common::Vector2d GetCoord(double sOffset, double tOffset) const override;
    virtual double GetDir(double sOffset) const override;

    //-----------------------------------------------------------------------------
    //! Returns the stored curvature.
    //!
    //! @return                         curvature
    //-----------------------------------------------------------------------------
    double GetCurvature() const
    {
        return curvature;
    }

  private:
    double curvature;
};

//-----------------------------------------------------------------------------
//! Class representing a spiral road (clothoid) as a RoadGeometry.
//-----------------------------------------------------------------------------
class RoadGeometrySpiral : public RoadGeometry
{
  public:
    RoadGeometrySpiral(double s, double x, double y, double hdg, double length, double curvStart, double curvEnd)
        : RoadGeometry{s, x, y, hdg, length}, curvStart{curvStart}, curvEnd{curvEnd}
    {
    }
    virtual ~RoadGeometrySpiral() override = default;
    virtual Common::Vector2d GetCoord(double sOffset, double tOffset) const override;
    virtual double GetDir(double sOffset) const override;

    //-----------------------------------------------------------------------------
    //! Returns the curvature at the start of the spiral.
    //!
    //! @return                         curvature at the start of the spiral
    //-----------------------------------------------------------------------------
    double GetCurvStart() const
    {
        return curvStart;
    }

    //-----------------------------------------------------------------------------
    //! Returns the curvature at the end of the spiral.
    //!
    //! @return                         curvature at the end of the spiral
    //-----------------------------------------------------------------------------
    double GetCurvEnd() const
    {
        return curvEnd;
    }

  private:
    //-----------------------------------------------------------------------------
    //! Calculates the x/y coordinates as vector. Only valid if the start and end
    //! curvature of the spiral are either both positive or negative.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  tOffset    offset to the left
    //! @return     vector with the x/y coordinates
    //-----------------------------------------------------------------------------
    Common::Vector2d HalfCoord(double sOffset, double tOffset) const;

    //-----------------------------------------------------------------------------
    //! Calculates the x/y coordinates as vector. Returns an empty vector, if start
    //! and end curvature of the spiral have different signs.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @param[in]  tOffset    offset to the left
    //! @return     vector with the x/y coordinates, empty vector,
    //!             if start and end curvature of the spiral have
    //!             different signs
    //-----------------------------------------------------------------------------
    Common::Vector2d FullCoord(double sOffset, double tOffset) const;

    //-----------------------------------------------------------------------------
    //! Calculates the curvature. Only valid if the start and end
    //! curvature of the spiral are either both positive or negative.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @return                curvature
    //-----------------------------------------------------------------------------
    double HalfCurvature(double sOffset) const;

    //-----------------------------------------------------------------------------
    //! Calculates the curvature. Returns 0.0, if start
    //! and end curvature of the spiral have different signs.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @return                curvature, 0.0, if start and end curvature
    //!                        of the spiral have different signs
    //-----------------------------------------------------------------------------
    double FullCurvature(double sOffset) const;

    //-----------------------------------------------------------------------------
    //! Calculates the direction. Only valid if the start and end
    //! curvature of the spiral are either both positive or negative.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @return                direction
    //-----------------------------------------------------------------------------
    double HalfDir(double sOffset) const;

    //-----------------------------------------------------------------------------
    //! Calculates the direction. Returns 0.0, if start
    //! and end curvature of the spiral have different signs.
    //!
    //! @param[in]  sOffset    s offset within geometry section
    //! @return                direction
    //-----------------------------------------------------------------------------
    double FullDir(double sOffset) const;

    double curvStart;
    double curvEnd;
};

//-----------------------------------------------------------------------------
//! Class representing a road form defined via a cubic polynomial as a RoadGeometry.
//-----------------------------------------------------------------------------
class RoadGeometryPoly3 : public RoadGeometry
{
  public:
    RoadGeometryPoly3(double s, double x, double y, double hdg, double length, double a, double b, double c, double d)
        : RoadGeometry(s, x, y, hdg, length), a(a), b(b), c(c), d(d)
    {
    }
    virtual ~RoadGeometryPoly3() override = default;
    virtual Common::Vector2d GetCoord(double sOffset, double tOffset) const override;
    virtual double GetDir(double sOffset) const override;

    //-----------------------------------------------------------------------------
    //! Returns the constant factor of the polynomial.
    //!
    //! @return                         constant factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetA() const
    {
        return a;
    }

    //-----------------------------------------------------------------------------
    //! Returns the linear factor of the polynomial.
    //!
    //! @return                         linear factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetB() const
    {
        return b;
    }

    //-----------------------------------------------------------------------------
    //! Returns the quadratic factor of the polynomial.
    //!
    //! @return                         quadratic factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetC() const
    {
        return c;
    }

    //-----------------------------------------------------------------------------
    //! Returns the cubic factor of the polynomial.
    //!
    //! @return                         cubic factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetD() const
    {
        return d;
    }

  private:
    double a;
    double b;
    double c;
    double d;
};

//-----------------------------------------------------------------------------
//! Class representing a road form defined via a parametric cubic polynomial as a RoadGeometry.
//-----------------------------------------------------------------------------
class RoadGeometryParamPoly3 : public RoadGeometry
{
  public:
    RoadGeometryParamPoly3(double s, double x, double y, double hdg, double length, ParamPoly3Parameters parameters)
        : RoadGeometry(s, x, y, hdg, length), parameters(parameters)
    {
    }
    virtual ~RoadGeometryParamPoly3() override = default;
    virtual Common::Vector2d GetCoord(double sOffset, double tOffset) const override;
    virtual double GetDir(double sOffset) const override;

    //-----------------------------------------------------------------------------
    //! Returns the constant factor of the polynomial for the u coordinate.
    //!
    //! @return                         constant factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetAU() const
    {
        return parameters.aU;
    }

    //-----------------------------------------------------------------------------
    //! Returns the linear factor of the polynomial for the u coordinate.
    //!
    //! @return                         linear factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetBU() const
    {
        return parameters.bU;
    }

    //-----------------------------------------------------------------------------
    //! Returns the quadratic factor of the polynomial for the u coordinate.
    //!
    //! @return                         quadratic factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetCU() const
    {
        return parameters.cU;
    }

    //-----------------------------------------------------------------------------
    //! Returns the cubic factor of the polynomial for the u coordinate.
    //!
    //! @return                         cubic factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetD() const
    {
        return parameters.dU;
    }

    //-----------------------------------------------------------------------------
    //! Returns the constant factor of the polynomial for the v coordinate.
    //!
    //! @return                         constant factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetAV() const
    {
        return parameters.aV;
    }

    //-----------------------------------------------------------------------------
    //! Returns the linear factor of the polynomial for the v coordinate.
    //!
    //! @return                         linear factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetBV() const
    {
        return parameters.bV;
    }

    //-----------------------------------------------------------------------------
    //! Returns the quadratic factor of the polynomial for the v coordinate.
    //!
    //! @return                         quadratic factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetCV() const
    {
        return parameters.cV;
    }

    //-----------------------------------------------------------------------------
    //! Returns the cubic factor of the polynomial for the v coordinate.
    //!
    //! @return                         cubic factor of the polynomial
    //-----------------------------------------------------------------------------
    double GetDV() const
    {
        return parameters.dV;
    }

  private:
    ParamPoly3Parameters parameters;
};

//-----------------------------------------------------------------------------
//! Class representing a road.
//-----------------------------------------------------------------------------
class Road : public RoadInterface
{
  public:
    Road(const std::string &id) : id(id)
    {
    }
    virtual ~Road() override;

    //-----------------------------------------------------------------------------
    //! Adds a line geometry to a road by creating a new RoadGeometryLine object and
    //! adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddGeometryLine(double s, double x, double y, double hdg, double length) override;

    //-----------------------------------------------------------------------------
    //! Adds an arc geometry to a road by creating a new RoadGeometryArc object and
    //! adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  curvature           constant curvature throughout the element
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddGeometryArc(double s, double x, double y, double hdg, double length, double curvature) override;

    //-----------------------------------------------------------------------------
    //! Adds a spiral geometry to a road by creating a new RoadGeometrySpiral object and
    //! adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  curvStart           curvature at the start of the element
    //! @param[in]  curvEnd             curvature at the end of the element
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddGeometrySpiral(double s, double x, double y, double hdg, double length, double curvStart,
                           double curvEnd) override;

    //-----------------------------------------------------------------------------
    //! Adds a cubic polynomial geometry to a road by creating a new RoadGeometryPoly3
    //! object and adding it to the stored list of geometries.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  a                   constant factor of the polynomial
    //! @param[in]  b                   linear factor of the polynomial
    //! @param[in]  c                   quadratic factor of the polynomial
    //! @param[in]  d                   cubic factor of the polynomial
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddGeometryPoly3(double s, double x, double y, double hdg, double length, double a, double b, double c,
                          double d) override;

    //-----------------------------------------------------------------------------
    //! Adds a parametric cubic polynomial geometry to a road by creating a new
    //! RoadGeometryparamPoly3 object and adding it to the stored list of geometries.
    //! Each coordinate is calculated in a local (u,v) coordinate system
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  x                   start position x inertial
    //! @param[in]  y                   start position y inertial
    //! @param[in]  hdg                 start orientation (inertial heading)
    //! @param[in]  length              length of the element's reference line
    //! @param[in]  parameters          Factors of the two polynomials describing the road
    //! @return                          false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddGeometryParamPoly3(double s, double x, double y, double hdg, double length,
                               ParamPoly3Parameters parameters) override;

    //-----------------------------------------------------------------------------
    //! Adds an elevation profile defined via a cubic polynomial to a road by creating
    //! a new RoadElevation object and adding it to the stored list of elevations.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  a                   constant factor of the polynomial
    //! @param[in]  b                   linear factor of the polynomial
    //! @param[in]  c                   quadratic factor of the polynomial
    //! @param[in]  d                   cubic factor of the polynomial
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddElevation(double s, double a, double b, double c, double d) override;

    //-----------------------------------------------------------------------------
    //! Adds a lane offset defined via a cubic polynomial to a road by creating a new
    //! RoadLaneOffset object and adding it to the stored list of lane offsets.
    //!
    //! @param[in]  s                   start position s-coordinate
    //! @param[in]  a                   constant factor of the polynomial
    //! @param[in]  b                   linear factor of the polynomial
    //! @param[in]  c                   quadratic factor of the polynomial
    //! @param[in]  d                   cubic factor of the polynomial
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddLaneOffset(double s, double a, double b, double c, double d) override;

    //-----------------------------------------------------------------------------
    //! Adds a new link to a road by creating a new RoadLink object and adding it
    //! to the stored list of links.
    //!
    //! @param[in]  type                type of link
    //! @param[in]  elementType         element type of the link
    //! @param[in]  elementId           ID of the linked element
    //! @param[in]  contactPoint        contact point type of link on the linked element
    //! @param[in]  direction           orientation of the lanes to link; only relevant
    //!                                 for a neighbor link type
    //! @param[in]  side                at which side the neighboring road lies; only
    //!                                 relevant for a neighbor link type
    //! @return                         false if an error has occurred, true otherwise
    //-----------------------------------------------------------------------------
    bool AddLink(RoadLinkType type, RoadLinkElementType elementType, const std::string &elementId,
                 ContactPointType contactPoint, RoadLinkDirectionType direction, RoadLinkSideType side) override;

    //-----------------------------------------------------------------------------
    //! Adds a new lane section to a road by creating a new RoadLaneSection object
    //! and adding it to the stored list of lane sections.
    //!
    //! @param[in]  start               start position s-coordinate
    //! @return                         created road lane section
    //-----------------------------------------------------------------------------
    RoadLaneSection *AddRoadLaneSection(double start) override;

    //-----------------------------------------------------------------------------
    //! Adds a new signal a road by creating a new RoadSignal object
    //! and adding it to the stored list of road signals.
    //!
    //! @return                         created road signal
    //-----------------------------------------------------------------------------
    void AddRoadSignal(const RoadSignalSpecification &signal) override;

    void AddRoadType(const RoadTypeSpecification &info) override;

    void AddRoadObject(const RoadObjectSpecification &object) override;

    RoadTypeInformation GetRoadType(double start) const override;

    //-----------------------------------------------------------------------------
    //! Returns the ID of the road.
    //!
    //! @return                         ID of the road
    //-----------------------------------------------------------------------------
    const std::string GetId() const override
    {
        return id;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of elevation profiles.
    //!
    //! @return                         list of elevation profiles
    //-----------------------------------------------------------------------------
    const std::list<RoadElevation *> &GetElevations() const override
    {
        return elevations;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of lane offsets.
    //!
    //! @return                         list of lane offsets
    //-----------------------------------------------------------------------------
    const std::list<RoadLaneOffset *> &GetLaneOffsets() const override
    {
        return laneOffsets;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of road geometries.
    //!
    //! @return                         list of road geometries
    //-----------------------------------------------------------------------------
    const std::list<RoadGeometryInterface *> &GetGeometries() const override
    {
        return geometries;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of road links.
    //!
    //! @return                         list of road links
    //-----------------------------------------------------------------------------
    const std::list<RoadLinkInterface *> &GetRoadLinks() const override
    {
        return links;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of lane sections.
    //!
    //! @return                         list of lane sections
    //-----------------------------------------------------------------------------
    const std::vector<RoadLaneSectionInterface *> &GetLaneSections() const override
    {
        return laneSections;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of signals
    //!
    //! @return                         list of signals
    //-----------------------------------------------------------------------------
    const std::vector<RoadSignalInterface *> &GetRoadSignals() const override
    {
        return roadSignals;
    }

    //-----------------------------------------------------------------------------
    //! Returns the stored list of objects
    //!
    //! @return                         list of objects
    //-----------------------------------------------------------------------------
    const std::vector<RoadObjectInterface *> &GetRoadObjects() const override
    {
        return roadObjects;
    }

    //-----------------------------------------------------------------------------
    //! Sets the flag, if the road is in the reference direction or not.
    //!
    //! @param[in]  inDirection         Flag, if the road is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    void SetInDirection(bool inDirection) override
    {
        this->inDirection = inDirection;
    }

    //-----------------------------------------------------------------------------
    //! Returns if the road is in the reference direction or not.
    //!
    //! @return                         Flag, if the road is in the reference
    //!                                 direction or not
    //-----------------------------------------------------------------------------
    bool GetInDirection() const override
    {
        return inDirection;
    }

    virtual void SetJunctionId(const std::string &junctionId) override
    {
        this->junctionId = junctionId;
    }

    const std::string GetJunctionId() const override
    {
        return junctionId;
    }

  private:
    const std::string id;
    std::list<RoadElevation *> elevations;
    std::list<RoadLaneOffset *> laneOffsets;
    std::list<RoadGeometryInterface *> geometries;
    std::list<RoadLinkInterface *> links;
    std::vector<RoadLaneSectionInterface *> laneSections;
    std::vector<RoadSignalInterface *> roadSignals;
    std::vector<RoadObjectInterface *> roadObjects;
    std::vector<RoadTypeSpecification> roadTypes;
    bool inDirection = true;
    std::string junctionId;
};
