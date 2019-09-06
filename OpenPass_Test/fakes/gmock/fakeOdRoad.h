/*********************************************************************
 * Copyright (c) 2018, 2019 in-tech
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 **********************************************************************/

#pragma once

#include "Interfaces/roadInterface/roadInterface.h"
#include "gmock/gmock.h"

class FakeOdRoad : public RoadInterface
{
  public:
    MOCK_METHOD5(AddGeometryLine, bool(double s, double x, double y, double hdg, double length));
    MOCK_METHOD6(AddGeometryArc, bool(double s, double x, double y, double hdg, double length, double curvature));
    MOCK_METHOD7(AddGeometrySpiral,
                 bool(double s, double x, double y, double hdg, double length, double curvStart, double curvEnd));
    MOCK_METHOD9(AddGeometryPoly3,
                 bool(double s, double x, double y, double hdg, double length, double a, double b, double c, double d));
    MOCK_METHOD6(AddGeometryParamPoly3,
                 bool(double s, double x, double y, double hdg, double length, ParamPoly3Parameters parameters));
    MOCK_METHOD5(AddElevation, bool(double s, double a, double b, double c, double d));
    MOCK_METHOD5(AddLaneOffset, bool(double s, double a, double b, double c, double d));
    MOCK_METHOD6(AddLink, bool(RoadLinkType, RoadLinkElementType, const std::string &, ContactPointType,
                               RoadLinkDirectionType, RoadLinkSideType));
    MOCK_METHOD1(AddRoadLaneSection, RoadLaneSectionInterface *(double start));
    MOCK_METHOD1(AddRoadSignal, void(const RoadSignalSpecification &signal));
    MOCK_METHOD1(AddRoadObject, void(const RoadObjectSpecification &object));
    MOCK_CONST_METHOD0(GetId, const std::string());
    MOCK_METHOD0(GetElevations, std::list<RoadElevation *> &());
    MOCK_METHOD0(GetLaneOffsets, std::list<RoadLaneOffset *> &());
    MOCK_CONST_METHOD0(GetLaneOffsets, const std::list<RoadLaneOffset *> &());
    MOCK_METHOD0(GetGeometries, std::list<RoadGeometryInterface *> &());
    MOCK_METHOD0(GetRoadLinks, std::list<RoadLinkInterface *> &());
    MOCK_METHOD0(GetLaneSections, std::vector<RoadLaneSectionInterface *> &());
    MOCK_METHOD0(GetRoadSignals, std::vector<RoadSignalInterface *> &());
    MOCK_METHOD0(GetRoadObjects, std::vector<RoadObjectInterface *> &());
    MOCK_METHOD1(SetInDirection, void(bool inDirection));
    MOCK_CONST_METHOD0(GetInDirection, bool());
    MOCK_METHOD1(AddRoadType, void(const RoadTypeSpecification &));
    MOCK_CONST_METHOD1(GetRoadType, RoadTypeInformation(double start));
    MOCK_METHOD0(GetJunctionId, std::string());
    MOCK_METHOD1(SetJunctionId, void(const std::string &id));
};
