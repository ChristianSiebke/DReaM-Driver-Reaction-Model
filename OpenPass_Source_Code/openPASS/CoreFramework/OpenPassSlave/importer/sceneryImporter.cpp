/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <algorithm>
#include <utility>
#include <limits>
#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include <QFile>

#include "CoreFramework/CoreShare/log.h"
#include "scenery.h"
#include "sceneryImporter.h"
#include "xmlParser.h"
#include "Common/commonTools.h"
#include "importerLoggingHelper.h"

using namespace Configuration;

namespace TAG = openpass::importer::xml::sceneryImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::sceneryImporter::attribute;

namespace Importer {
const std::map<std::string, RoadSignalUnit> roadSignalUnitConversionMap = {
                                                                            {"m", RoadSignalUnit::Meter},
                                                                            {"km", RoadSignalUnit::Kilometer},
                                                                            {"ft", RoadSignalUnit::Feet},
                                                                            {"mile", RoadSignalUnit::LandMile},
                                                                            {"m/s", RoadSignalUnit::MetersPerSecond},
                                                                            {"km/h", RoadSignalUnit::KilometersPerHour},
                                                                            {"mph", RoadSignalUnit::MilesPerHour},
                                                                            {"kg", RoadSignalUnit::Kilogram},
                                                                            {"t", RoadSignalUnit::MetricTons},
                                                                            {"%", RoadSignalUnit::Percent}
                                                                          };
void SceneryImporter::ParseLanes(QDomElement& rootElement,
                                 RoadLaneSectionInterface* laneSection)
{
    QDomElement roadLaneElement;

    QDomNode node = rootElement.firstChildElement("lane");

    if (node.isNull())
    {
        return;
    }

    roadLaneElement = node.toElement();
    ThrowIfFalse(!roadLaneElement.isNull(), "Road lane element is null.");

    while (!roadLaneElement.isNull())
    {
        int roadLaneId;
        ThrowIfFalse(SimulationCommon::ParseAttributeInt(roadLaneElement, ATTRIBUTE::id, roadLaneId),
                      "Could not parse lanes. RoadLane tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");

        std::string roadLaneTypeStr;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLaneElement, ATTRIBUTE::type, roadLaneTypeStr),
                      "Could not parse lanes. RoadLane tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");

        RoadLaneType roadLaneType = roadLaneTypeConversionMap.at(roadLaneTypeStr);

        RoadLaneInterface* roadLane = laneSection->AddRoadLane(roadLaneId,
                                      roadLaneType);
        ThrowIfFalse(roadLane, "roadLane is null");
        LOG_INTERN(LogLevel::DebugCore) << "lane: type " << static_cast<int>(roadLaneType) << ", id=" << roadLaneId;

        if (0 != roadLaneId) // skip center lanes
        {
            QDomElement roadLaneWidthElement;
            // at least one width element necessary
            ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadLaneElement, TAG::width, roadLaneWidthElement),
                          "Could not parse lanes. Tag " + std::string(TAG::width) + " is missing.");
            while (!roadLaneWidthElement.isNull())
            {
                double roadLaneSOffset, roadLaneA, roadLaneB, roadLaneC, roadLaneD;
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneWidthElement, ATTRIBUTE::sOffset, roadLaneSOffset),
                              "Could not parse lanes. Width tag requires a " + std::string(ATTRIBUTE::sOffset) + " attribute.");
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneWidthElement, ATTRIBUTE::a, roadLaneA),
                              "Could not parse lanes. Width tag requires a " + std::string(ATTRIBUTE::a) + " attribute.");
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneWidthElement, ATTRIBUTE::b, roadLaneB),
                              "Could not parse lanes. Width tag requires a " + std::string(ATTRIBUTE::b) + " attribute.");
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneWidthElement, ATTRIBUTE::c, roadLaneC),
                              "Could not parse lanes. Width tag requires a " + std::string(ATTRIBUTE::c) + " attribute.");
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneWidthElement, ATTRIBUTE::d, roadLaneD),
                              "Could not parse lanes. Width tag requires a " + std::string(ATTRIBUTE::ds) + " attribute.");

                roadLane->AddWidth(roadLaneSOffset,
                                   roadLaneA,
                                   roadLaneB,
                                   roadLaneC,
                                   roadLaneD);

                roadLaneWidthElement = roadLaneWidthElement.nextSiblingElement(TAG::width);
            }
        }

        QDomElement roadLaneLinkElement;
        if (SimulationCommon::GetFirstChildElement(roadLaneElement, TAG::link, roadLaneLinkElement))
        {
            int roadLaneLinkItemId;
            QDomElement roadLaneLinkItemElement;
            if (SimulationCommon::GetFirstChildElement(roadLaneLinkElement, TAG::successor, roadLaneLinkItemElement))
            {
                ThrowIfFalse(SimulationCommon::ParseAttributeInt(roadLaneLinkItemElement, ATTRIBUTE::id, roadLaneLinkItemId),
                              "Could not parse lanes. Link tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");
                roadLane->AddSuccessor(roadLaneLinkItemId);
            }

            if (SimulationCommon::GetFirstChildElement(roadLaneLinkElement, TAG::predecessor, roadLaneLinkItemElement))
            {
                ThrowIfFalse(SimulationCommon::ParseAttributeInt(roadLaneLinkItemElement, ATTRIBUTE::id, roadLaneLinkItemId),
                              "Could not parse lanes. Link tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");
                roadLane->AddPredecessor(roadLaneLinkItemId);
            }
        }

        ParseLaneRoadMark(rootElement.nodeName().toStdString(), roadLaneElement, roadLane);


        roadLaneElement = roadLaneElement.nextSiblingElement(TAG::lane);
    } // lane loop
}

void SceneryImporter::ParseLaneRoadMark(std::string leftCenterRight, QDomElement& roadLaneElement,
                                        RoadLaneInterface* roadLane)
{
    RoadLaneRoadDescriptionType descType = RoadLaneRoadDescriptionType::Center;

    if (0 == leftCenterRight.compare("left"))
    {
        descType = RoadLaneRoadDescriptionType::Left;
    }
    else if (0 == leftCenterRight.compare("right"))
    {
        descType = RoadLaneRoadDescriptionType::Right;
    }

    QDomElement roadLaneRoadMarkElement;
    SimulationCommon::GetFirstChildElement(roadLaneElement, TAG::roadMark, roadLaneRoadMarkElement);

    while (!roadLaneRoadMarkElement.isNull())
    {
        double roadLaneSOffset;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneRoadMarkElement, ATTRIBUTE::sOffset, roadLaneSOffset, 0.0),
                      "Could not parse lane road mark. RoadMark tag requires a " + std::string(ATTRIBUTE::sOffset) + " attribute.");

        std::string roadMarkTypeStr;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLaneRoadMarkElement, ATTRIBUTE::type, roadMarkTypeStr, "none"),
                      "Could not parse lane road mark. RoadMark tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");
        RoadLaneRoadMarkType roadMarkType = RoadLaneRoadMarkType::Undefined;
        if (roadMarkTypeStr == "none")
        {
            roadMarkType = RoadLaneRoadMarkType::None;
        }
        else if (roadMarkTypeStr == "solid")
        {
            roadMarkType = RoadLaneRoadMarkType::Solid;
        }
        else if (roadMarkTypeStr == "broken")
        {
            roadMarkType = RoadLaneRoadMarkType::Broken;
        }
        else if (roadMarkTypeStr == "solid solid")
        {
            roadMarkType = RoadLaneRoadMarkType::Solid_Solid;
        }
        else if (roadMarkTypeStr == "solid broken")
        {
            roadMarkType = RoadLaneRoadMarkType::Solid_Broken;
        }
        else if (roadMarkTypeStr == "broken solid")
        {
            roadMarkType = RoadLaneRoadMarkType::Broken_Solid;
        }
        else if (roadMarkTypeStr == "broken broken")
        {
            roadMarkType = RoadLaneRoadMarkType::Broken_Broken;
        }
        else if (roadMarkTypeStr == "botts dots")
        {
            roadMarkType = RoadLaneRoadMarkType::Botts_Dots;
        }
        else if (roadMarkTypeStr == "grass")
        {
            roadMarkType = RoadLaneRoadMarkType::Grass;
        }
        else if (roadMarkTypeStr == "curb")
        {
            roadMarkType = RoadLaneRoadMarkType::Curb;
        }

        std::string roadMarkColorStr;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLaneRoadMarkElement, ATTRIBUTE::color, roadMarkColorStr, "standard"),
                      "Could not parse lane road mark. RoadMark tag requires a " + std::string(ATTRIBUTE::color) + " attribute.");
        RoadLaneRoadMarkColor color = RoadLaneRoadMarkColor::Undefined;
        if (roadMarkColorStr == "standard" || roadMarkColorStr == "white")
        {
            color = RoadLaneRoadMarkColor::White;
        }
        else if (roadMarkColorStr == "yellow")
        {
            color = RoadLaneRoadMarkColor::Yellow;
        }
        else if (roadMarkColorStr == "blue")
        {
            color = RoadLaneRoadMarkColor::Blue;
        }
        else if (roadMarkColorStr == "green")
        {
            color = RoadLaneRoadMarkColor::Green;
        }
        else if (roadMarkColorStr == "red")
        {
            color = RoadLaneRoadMarkColor::Red;
        }
        else if (roadMarkColorStr == "orange")
        {
            color = RoadLaneRoadMarkColor::Orange;
        }

        RoadLaneRoadMarkLaneChange roadChange = RoadLaneRoadMarkLaneChange::Undefined;

        std::string weightStr;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLaneRoadMarkElement, ATTRIBUTE::weight, weightStr, "standard"),
                      "Could not parse lane road mark. RoadMark tag requires a " + std::string(ATTRIBUTE::weight) + " attribute.");
        RoadLaneRoadMarkWeight weight = RoadLaneRoadMarkWeight::Undefined;
        if (weightStr == "standard")
        {
            weight = RoadLaneRoadMarkWeight::Standard;
        }
        else if (weightStr == "bold")
        {
            weight = RoadLaneRoadMarkWeight::Bold;
        }

        roadLane->AddRoadMark(roadLaneSOffset, descType, roadMarkType, color, roadChange, weight);

        roadLaneRoadMarkElement = roadLaneRoadMarkElement.nextSiblingElement(TAG::roadMark);
    }
}

void SceneryImporter::ParseGeometries(QDomElement& roadElement,
                                      RoadInterface* road)
{
    // road plan view
    QDomElement roadPlanView;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadElement, TAG::planView, roadPlanView),
                  "Could not parse geometries. Tag " + std::string(TAG::planView) + " is missing.");

    QDomElement roadGeometryHeaderElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadPlanView, TAG::geometry, roadGeometryHeaderElement),
                  "Could not parse geometries. Tag " + std::string(TAG::geometry) + " is missing.");
    while (!roadGeometryHeaderElement.isNull())
    {
        double roadGeometryS;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::s, roadGeometryS),
                      "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");
        roadGeometryS = CommonHelper::roundDoubleWithDecimals(roadGeometryS, 3);

        double roadGeometryX;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::x, roadGeometryX),
                      "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::x) + " attribute.");
        roadGeometryX = CommonHelper::roundDoubleWithDecimals(roadGeometryX, 3);

        double roadGeometryY;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::y, roadGeometryY),
                      "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::y) + " attribute.");
        roadGeometryY = CommonHelper::roundDoubleWithDecimals(roadGeometryY, 3);

        double roadGeometryHdg;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::hdg, roadGeometryHdg),
                      "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::hdg) + " attribute.");
        roadGeometryHdg = CommonHelper::roundDoubleWithDecimals(roadGeometryHdg, 6);

        double roadGeometryLength;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::length, roadGeometryLength),
                      "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::length) + " attribute.");
        roadGeometryLength = CommonHelper::roundDoubleWithDecimals(roadGeometryLength, 3);

        QDomElement roadGeometryElement;
        if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::line, roadGeometryElement))
        {
           ThrowIfFalse(road->AddGeometryLine(roadGeometryS,
                                             roadGeometryX,
                                             roadGeometryY,
                                             roadGeometryHdg,
                                             roadGeometryLength),
                         "Unable to add geometry line.");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::arc, roadGeometryElement))
        {
            double roadGeometryCurvature;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::curvature,
                       roadGeometryCurvature),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::curvature) + " attribute.");
            ThrowIfFalse(road->AddGeometryArc(roadGeometryS,
                                            roadGeometryX,
                                            roadGeometryY,
                                            roadGeometryHdg,
                                            roadGeometryLength,
                                            roadGeometryCurvature),
                          "Unable to add geometry arc");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::spiral, roadGeometryElement))
        {
            double roadGeometryCurvStart;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::curvStart,
                       roadGeometryCurvStart),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::curvStart) + " attribute.");

            double roadGeometryCurvEnd;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::curvEnd,
                       roadGeometryCurvEnd),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::curvEnd) + " attribute.");

            if (roadGeometryCurvStart < 1e-7 && roadGeometryCurvStart > -1e-7)
            {
                roadGeometryCurvStart = 0;
            }

            if (roadGeometryCurvEnd < 1e-7 && roadGeometryCurvEnd > -1e-7)
            {
                roadGeometryCurvEnd = 0;
            }
            ThrowIfFalse(road->AddGeometrySpiral(roadGeometryS,
                                               roadGeometryX,
                                               roadGeometryY,
                                               roadGeometryHdg,
                                               roadGeometryLength,
                                               roadGeometryCurvStart,
                                               roadGeometryCurvEnd),
                          "Unable to add geometry spiral.");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::poly3, roadGeometryElement))
        {
            double roadGeometryA;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::a,
                       roadGeometryA),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::a) + " attribute.");

            double roadGeometryB;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::b,
                       roadGeometryB),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::b) + " attribute.");

            double roadGeometryC;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::c,
                       roadGeometryC),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::c) + " attribute.");

            double roadGeometryD;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::d,
                       roadGeometryD),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::d) + " attribute.");
            ThrowIfFalse(road->AddGeometryPoly3(roadGeometryS,
                                              roadGeometryX,
                                              roadGeometryY,
                                              roadGeometryHdg,
                                              roadGeometryLength,
                                              roadGeometryA,
                                              roadGeometryB,
                                              roadGeometryC,
                                              roadGeometryD),
                          "Unable to add geometry poly3.");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::paramPoly3, roadGeometryElement))
        {
            double roadGeometryaU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::aU,
                       roadGeometryaU),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::aU) + " attribute.");

            double roadGeometrybU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::bU,
                       roadGeometrybU),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::bU) + " attribute.");

            double roadGeometrycU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::cU,
                       roadGeometrycU),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::cU) + " attribute.");

            double roadGeometrydU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::dU,
                       roadGeometrydU),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::dU) + " attribute.");

            double roadGeometryaV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::aV,
                       roadGeometryaV),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::aV) + " attribute.");

            double roadGeometrybV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::bV,
                       roadGeometrybV),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::bV) + " attribute.");

            double roadGeometrycV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::cV,
                       roadGeometrycV),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::cV) + " attribute.");

            double roadGeometrydV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement,
                       ATTRIBUTE::dV,
                       roadGeometrydV),
                          "Could not parse geometries. Geometry tag requires a " + std::string(ATTRIBUTE::dV) + " attribute.");

            ParamPoly3Parameters parameters;
            parameters = {roadGeometryaU, roadGeometrybU, roadGeometrycU, roadGeometrydU, roadGeometryaV, roadGeometrybV, roadGeometrycV, roadGeometrydV};
            ThrowIfFalse(road->AddGeometryParamPoly3(roadGeometryS,
                                                   roadGeometryX,
                                                   roadGeometryY,
                                                   roadGeometryHdg,
                                                   roadGeometryLength,
                                                   parameters),
                          "Unable to add geometry param poly3.");
        }
        else
        {
            LogErrorAndThrow("invalid geometry");
        }

        LOG_INTERN(LogLevel::DebugCore) << "road geometry: s: " << roadGeometryS
                                        << ", x: " << roadGeometryX
                                        << ", y: " << roadGeometryY
                                        << ", hdg: " << roadGeometryHdg
                                        << ", length: " << roadGeometryLength;

        roadGeometryHeaderElement = roadGeometryHeaderElement.nextSiblingElement(TAG::geometry);
    } // road geometry loop
}

void SceneryImporter::ParseElevationProfile(QDomElement& roadElement,
        RoadInterface* road)
{
    QDomElement elevationProfileElement;
    if (SimulationCommon::GetFirstChildElement(roadElement, TAG::elevationProfile, elevationProfileElement))
    {
        QDomElement elevationElement;
        if (SimulationCommon::GetFirstChildElement(elevationProfileElement, TAG::elevation, elevationElement))
        {
            while (!elevationElement.isNull())
            {
                double elevationS, elevationA, elevationB, elevationC, elevationD;
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement,
                           ATTRIBUTE::s,
                           elevationS),
                              "Could not parse elevation profile. Elevation tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement,
                           ATTRIBUTE::a,
                           elevationA),
                              "Could not parse elevation profile. Elevation tag requires a " + std::string(ATTRIBUTE::a) + " attribute.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement,
                           ATTRIBUTE::b,
                           elevationB),
                              "Could not parse elevation profile. Elevation tag requires a " + std::string(ATTRIBUTE::b) + " attribute.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement,
                           ATTRIBUTE::c,
                           elevationC),
                              "Could not parse elevation profile. Elevation tag requires a " + std::string(ATTRIBUTE::c) + " attribute.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement,
                           ATTRIBUTE::d,
                           elevationD),
                              "Could not parse elevation profile. Elevation tag requires a " + std::string(ATTRIBUTE::d) + " attribute.");

                road->AddElevation(elevationS,
                                   elevationA,
                                   elevationB,
                                   elevationC,
                                   elevationD);

                elevationElement = elevationElement.nextSiblingElement(TAG::elevation);
            } // elevation loop
        } // if elevations exist
    } // if elevation profiles exist
}

void SceneryImporter::ParseRoadLinks(QDomElement& roadElement,
                                     RoadInterface* road)
{
    QDomElement roadLinkElement;
    if (SimulationCommon::GetFirstChildElement(roadElement, TAG::link, roadLinkElement))
    {
        QDomElement roadLinkItemElement;
        if (SimulationCommon::GetFirstChild(roadLinkElement, roadLinkItemElement))
        {
            while (!roadLinkItemElement.isNull())
            {
                RoadLinkType roadLinkType = RoadLinkType::Undefined;
                RoadLinkElementType roadLinkElementType = RoadLinkElementType::Undefined;
                std::string roadLinkElementId;
                ContactPointType roadLinkContactPoint = ContactPointType::Undefined;
                RoadLinkDirectionType roadLinkDirection = RoadLinkDirectionType::Undefined;
                RoadLinkSideType roadLinkSide = RoadLinkSideType::Undefined;

                if (0 == roadLinkItemElement.tagName().compare(TAG::predecessor) ||
                        0 == roadLinkItemElement.tagName().compare(TAG::successor))
                {
                    if (0 == roadLinkItemElement.tagName().compare(TAG::predecessor))
                    {
                        roadLinkType = RoadLinkType::Predecessor;
                    }
                    else
                    {
                        roadLinkType = RoadLinkType::Successor;
                    }

                    std::string roadLinkItem_ElementTypeAttribute;
                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::elementType,
                               roadLinkItem_ElementTypeAttribute),
                                  "Could not parse road links. LinkItem tag requires a " + std::string(ATTRIBUTE::elementType) + " attribute.");

                    if (0 == roadLinkItem_ElementTypeAttribute.compare(ATTRIBUTE::road))
                    {
                        roadLinkElementType = RoadLinkElementType::Road;
                    }
                    else if (0 == roadLinkItem_ElementTypeAttribute.compare(ATTRIBUTE::junction))
                    {
                        roadLinkElementType = RoadLinkElementType::Junction;
                    }
                    else
                    {
                        LogErrorAndThrow("invalid road link attribute");
                    }

                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::elementId, roadLinkElementId),
                                  "Could not parse road links. LinkItem tag requires a " + std::string(ATTRIBUTE::elementId) + " attribute.");

                    if (RoadLinkElementType::Road == roadLinkElementType)
                    {
                        std::string roadLinkItem_ContactPointAttribute;
                        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::contactPoint,
                                   roadLinkItem_ContactPointAttribute),
                                      "Could not parse road links. LinkItem tag requries a " + std::string(ATTRIBUTE::contactPoint) + " attribute.");

                        if (0 == roadLinkItem_ContactPointAttribute.compare(ATTRIBUTE::start))
                        {
                            roadLinkContactPoint = ContactPointType::Start;
                        }
                        else if (0 == roadLinkItem_ContactPointAttribute.compare(ATTRIBUTE::end))
                        {
                            roadLinkContactPoint = ContactPointType::End;
                        }
                        else
                        {
                            LogErrorAndThrow("invalid road link attribute");
                        }
                    }
                }
                else if (0 == roadLinkItemElement.tagName().compare(TAG::neighbor))
                {
                    roadLinkType = RoadLinkType::Neighbor;

                    std::string roadLinkItem_SideAttribute;
                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::side, roadLinkItem_SideAttribute),
                                  "Could not parse road links. LinkItem tag requires a " + std::string(ATTRIBUTE::side) + " attribute.");

                    if (0 == roadLinkItem_SideAttribute.compare(ATTRIBUTE::left))
                    {
                        roadLinkSide = RoadLinkSideType::Left;
                    }
                    else if (0 == roadLinkItem_SideAttribute.compare(ATTRIBUTE::right))
                    {
                        roadLinkSide = RoadLinkSideType::Right;
                    }
                    else
                    {
                        LogErrorAndThrow("invalid road link attribute.");
                    }

                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::elementId, roadLinkElementId),
                                  "Could not parse road links. LinkItem tag requires a " + std::string(ATTRIBUTE::elementId) + " attribute.");

                    std::string roadLinkItem_DirectionAttribute;
                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::direction, roadLinkItem_DirectionAttribute),
                                  "Could not parse road links. LinkItem tag requires a " + std::string(ATTRIBUTE::direction) + " attribute.");

                    if (0 == roadLinkItem_DirectionAttribute.compare(ATTRIBUTE::same))
                    {
                        roadLinkDirection = RoadLinkDirectionType::Same;
                    }
                    else if (0 == roadLinkItem_DirectionAttribute.compare(ATTRIBUTE::opposite))
                    {
                        roadLinkDirection = RoadLinkDirectionType::Opposite;
                    }
                    else
                    {
                        LogErrorAndThrow("invalid road link attribute");
                    }
                }
                else
                {
                    LogErrorAndThrow("invalid road link type");
                }

                LOG_INTERN(LogLevel::DebugCore) << "roadlink: " << roadLinkItemElement.tagName().toStdString() << " (" <<
                                                (int)roadLinkElementType << ", " << roadLinkElementId << ", " << (int)roadLinkContactPoint << ", " <<
                                                (int)roadLinkDirection << ", " << (int)roadLinkSide << ")";

                ThrowIfFalse(road->AddLink(roadLinkType,
                                         roadLinkElementType,
                                         roadLinkElementId,
                                         roadLinkContactPoint,
                                         roadLinkDirection,
                                         roadLinkSide),
                              "Unable to add road link.");

                roadLinkItemElement = roadLinkItemElement.nextSiblingElement(); // any type of road link element
            } // road link item loop
        } // if road link item exists
    } // if road links exist
}

void SceneryImporter::checkRoadSignalBoundaries(RoadSignalSpecification signal)
{
    ThrowIfFalse((signal.s >= 0 &&
                 (signal.dynamic == "yes" || signal.dynamic == "no") &&
                 (signal.orientation == "+" || signal.orientation == "-" || signal.orientation == "none") &&
                 !signal.country.empty() &&
                 signal.height >= 0 &&
                 signal.width >= 0),
            "Invalid road signal boundaries.");
;
}

void SceneryImporter::ParseSignals(QDomElement& roadElement,
                                   RoadInterface* road)
{
    using namespace SimulationCommon;

    QDomElement signalsElement;
    if (GetFirstChildElement(roadElement, TAG::Signals, signalsElement))
    {
        QDomElement signalElement;
        if (GetFirstChildElement(signalsElement, TAG::signal, signalElement))
        {
            while (!signalElement.isNull())
            {
                RoadSignalSpecification signal;

                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::s,           signal.s),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");
                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::t,           signal.t),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::t) + " attribute.");
                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::zOffset,     signal.zOffset),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::zOffset) + " attribute.");
                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::hOffset,     signal.hOffset),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::hOffset) + " attribute.");
                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::pitch,       signal.pitch),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::pitch) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::id,          signal.id),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::name,        signal.name),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::name) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::dynamic,     signal.dynamic, "no"),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::dynamic) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::orientation, signal.orientation),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::orientation) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::country,     signal.country),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::country) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::type,        signal.type),
                        "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::subtype,     signal.subtype),
                              "Could not parse signals. Signal tag requires a " + std::string(ATTRIBUTE::subtype) + " attribute.");
                // optional
                std::string signalUnit;
                ParseAttributeDouble(signalElement, ATTRIBUTE::value, signal.value);
                ParseAttributeString(signalElement, ATTRIBUTE::unit, signalUnit);
                ParseSignalUnit(signalUnit, signal.unit);

                // not generated by ODDlot
                // potentially optional - no clue from the standard
                ParseAttributeDouble(signalElement, ATTRIBUTE::height, signal.height);
                ParseAttributeDouble(signalElement, ATTRIBUTE::width,  signal.width);
                ParseAttributeString(signalElement, ATTRIBUTE::text,   signal.text);

                // check validity subtag
                ParseElementValidity(signalElement, signal.validity);

                // check other parameters
                checkRoadSignalBoundaries(signal);

                // check dependencies
                QDomElement dependencyElement;
                if (GetFirstChildElement(signalElement, TAG::dependency, dependencyElement))
                {
                    std::string dependencyId {};

                    while (!dependencyElement.isNull())
                    {
                        ThrowIfFalse(ParseAttributeString(dependencyElement, ATTRIBUTE::id, dependencyId),
                                      "Could not parse signals. Dependency tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");
                        signal.dependencyIds.push_back(dependencyId);

                        dependencyElement = dependencyElement.nextSiblingElement(TAG::dependency);
                    }
                }

                road->AddRoadSignal(signal);

                signalElement = signalElement.nextSiblingElement(TAG::signal);
            }
        }
    }
}

void SceneryImporter::ParseObjects(QDomElement& roadElement, RoadInterface* road)
{
    using namespace SimulationCommon;

    QDomElement objectsElement;
    if (GetFirstChildElement(roadElement, TAG::objects, objectsElement))
    {
        QDomElement objectElement;
        if (GetFirstChildElement(objectsElement, TAG::object, objectElement))
        {
            while (!objectElement.isNull())
            {
                ParseObject(objectElement, road);

                objectElement = objectElement.nextSiblingElement(TAG::object);
            }
        }
    }
}

void SceneryImporter::ParseObject(QDomElement& objectElement, RoadInterface* road)
{
    using namespace SimulationCommon;

    RoadObjectSpecification object;

    ThrowIfFalse(ParseAttributeType(objectElement, ATTRIBUTE::type, object.type),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");
    ThrowIfFalse(ParseAttributeString(objectElement, ATTRIBUTE::name, object.name),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::name) + " attribute.");
    ThrowIfFalse(ParseAttributeString(objectElement, ATTRIBUTE::id, object.id),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::s, object.s),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::t, object.t),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::t) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::zOffset, object.zOffset),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::zOffset) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::validLength, object.validLength),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::validLength) + " attribute.");
    ThrowIfFalse(ParseAttributeType(objectElement, ATTRIBUTE::orientation, object.orientation),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::orientation) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::width, object.width),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::width) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::length, object.length),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::length) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::height, object.height),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::height) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::hdg, object.hdg),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::hdg) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::pitch, object.pitch),
                  "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::pitch) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::roll, object.roll),
                 "Could not parse object. Object tag requires a " + std::string(ATTRIBUTE::roll) + " attribute.");
    ParseAttributeDouble(objectElement, "radius", object.radius);

    ThrowIfFalse(object.checkStandardCompliance(), "limits of object " + object.id + " " + object.name + " are not valid for openDrive standard");

    ThrowIfFalse(object.checkSimulatorCompliance(), "Limits of object " + object.id + " " + object.name + " are not valid for the simulation. The Object will be ignored.");

    ParseElementValidity(objectElement, object.validity);

    std::list<RoadObjectSpecification> parsedObjectRepetitions = ParseObjectRepeat(objectElement, object);
    AddParsedObjectsToRoad(parsedObjectRepetitions, road);

}

void SceneryImporter::AddParsedObjectsToRoad(std::list<RoadObjectSpecification> parsedObjects, RoadInterface* road)
{
    for (auto object : parsedObjects)
    {
        road->AddRoadObject(object);
    }
}

std::list<RoadObjectSpecification> SceneryImporter::ParseObjectRepeat(QDomElement& objectElement,
        const RoadObjectSpecification& object)
{
    using namespace SimulationCommon;

    std::list<RoadObjectSpecification> objectRepetitions;

    QDomElement repeatElement;
    if (GetFirstChildElement(objectElement, TAG::repeat, repeatElement))
    {
        while (!repeatElement.isNull())
        {
            ParseRepeat(repeatElement, object, objectRepetitions);
            repeatElement = repeatElement.nextSiblingElement(TAG::repeat);
        }
    }
    else
    {
        objectRepetitions.push_back(object);
    }
    return objectRepetitions;
}

void SceneryImporter::ParseOptionalInterval(QDomElement& repeatElement, std::string startAttribute,
        std::string endAttribute, OptionalInterval& interval)
{
    bool checkStartIsSet = SimulationCommon::ParseAttributeDouble(repeatElement, startAttribute, interval.start);
    bool checkEndIsSet = SimulationCommon::ParseAttributeDouble(repeatElement, endAttribute, interval.end);

    if (checkStartIsSet && checkEndIsSet)
    {
        interval.isSet = true;
        return;
    }
    ThrowIfFalse(!checkStartIsSet && !checkEndIsSet, "Missing intervall parameter in scenery import");
    return;
}

void SceneryImporter::ParseRepeat(QDomElement& repeatElement, RoadObjectSpecification object,
                                  std::list<RoadObjectSpecification>& objectRepitions)
{
    using namespace SimulationCommon;
    ObjectRepeat repeat;

    ThrowIfFalse(ParseAttributeDouble(repeatElement, ATTRIBUTE::s,            repeat.s),
                  "Could not parse repeat. Repeat tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(repeatElement, ATTRIBUTE::length,       repeat.length),
                  "Could not parse repeat. Repeat tag requires a " + std::string(ATTRIBUTE::length) + " attribute.");
    ThrowIfFalse(ParseAttributeDouble(repeatElement, ATTRIBUTE::distance,     repeat.distance),
                  "Could not parse repeat. Repeat tag requires a " + std::string(ATTRIBUTE::distance) + " attribute.");

    ParseOptionalInterval(repeatElement, "tStart", "tEnd", repeat.t);
    ParseOptionalInterval(repeatElement, "widthStart", "widthEnd", repeat.width);
    ParseOptionalInterval(repeatElement, "heightStart", "heightEnd", repeat.height);
    ParseOptionalInterval(repeatElement, "zOffsetStart", "zOffsetEnd", repeat.zOffset);

    ThrowIfFalse(repeat.checkLimits(), "Invalid limits.");

    return ApplyRepeat(repeat, object, objectRepitions);
}

void SceneryImporter::ApplyRepeat(ObjectRepeat repeat, RoadObjectSpecification object,
                                  std::list<RoadObjectSpecification>& objectRepitions)
{
    if (repeat.distance == 0)
    {
        repeat.distance = object.length;
    }

    int objectCount = int(repeat.length / repeat.distance);

    std::vector<double> interpolatedT, interpolatedHeight, interpolatedWidth, interpolatedZOffset;

    if (repeat.t.isSet) { interpolatedT = CommonHelper::InterpolateLinear(repeat.t.start,  repeat.t.end, objectCount); }
    if (repeat.height.isSet) { interpolatedHeight = CommonHelper::InterpolateLinear(repeat.height.start, repeat.height.end, objectCount); }
    if (repeat.width.isSet) { interpolatedWidth = CommonHelper::InterpolateLinear(repeat.width.start, repeat.width.end, objectCount); }
    if (repeat.zOffset.isSet) { interpolatedZOffset = CommonHelper::InterpolateLinear(repeat.zOffset.start, repeat.zOffset.end, objectCount); }

    for (int i = 0; i < objectCount; i++)
    {
        RoadObjectSpecification repeatingObject = object;
        repeatingObject.s = repeat.s + (i * repeat.distance);

        if (repeat.t.isSet) { repeatingObject.t = interpolatedT.at(i); }
        if (repeat.height.isSet) { repeatingObject.height = interpolatedHeight.at(i); }
        if (repeat.width.isSet) { repeatingObject.width = interpolatedWidth.at(i); }
        if (repeat.zOffset.isSet) { repeatingObject.zOffset = interpolatedZOffset.at(i); }

        ThrowIfFalse(repeatingObject.checkStandardCompliance(), "Standard compliance invalid.");

        ThrowIfFalse(repeatingObject.checkSimulatorCompliance(), "Limits of repeating object are not valid for the simulation. The Object will be ignored.");

        objectRepitions.push_back(repeatingObject);
    }
}

void SceneryImporter::ParseElementValidity(const QDomElement& rootElement, RoadElementValidity& validity)
{
    using namespace SimulationCommon;

    QDomElement validityElement;
    if (GetFirstChildElement(rootElement, TAG::validity, validityElement))
    {
        int fromLane;
        int toLane;

        ThrowIfFalse(ParseAttributeInt(validityElement, ATTRIBUTE::fromLane, fromLane),
                      "Could not parse validity. Validity tag requires a " + std::string(ATTRIBUTE::fromLane) + " attribute.");
        ThrowIfFalse(ParseAttributeInt(validityElement, ATTRIBUTE::toLane,   toLane),
                      "Could not parse validity. Validity tag requires a " + std::string(ATTRIBUTE::toLane) + " attribute.");

        if (fromLane > toLane)
        {
            std::swap(fromLane, toLane);
        }

        for (int laneId = fromLane; laneId <= toLane; laneId++)
        {
            validity.lanes.push_back(laneId);
        }
    }
    else
    {
        validity.all = true;
    }
}

void SceneryImporter::ParseRoadTypes(QDomElement& roadElement, RoadInterface* road)
{
    using namespace SimulationCommon;


    QDomElement typeElement;
    if (GetFirstChildElement(roadElement, TAG::type, typeElement))
    {
        while (!typeElement.isNull())
        {
            RoadTypeSpecification roadType;
            ThrowIfFalse(ParseAttributeDouble(typeElement, ATTRIBUTE::s,           roadType.s),
                          "Could not parse road types. Type tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");
            std::string roadTypeStr;
            ThrowIfFalse(ParseAttributeString(typeElement, ATTRIBUTE::type,           roadTypeStr),
                          "Coult not parse road types. Type tag requires a " + std::string(ATTRIBUTE::type) + " attribute.");

            roadType.roadType = roadTypeConversionMap.at(roadTypeStr);


            road->AddRoadType(roadType);

            typeElement = typeElement.nextSiblingElement(TAG::type);
        }
    }
}

void SceneryImporter::ParseSignalUnit(std::string element, RoadSignalUnit& signalUnit)
{
    // empty is a valid state!
    if (element.empty())
    {
        signalUnit = RoadSignalUnit::Undefined;
    }
    else
    {
        signalUnit = roadSignalUnitConversionMap.at(element);
    }
}

void SceneryImporter::ParseRoadLanes(QDomElement& roadElement,
                                     RoadInterface* road)
{
    QDomElement roadLanesElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadElement, TAG::lanes, roadLanesElement),
                  "Could not parse road lanes. Tag + " + std::string(TAG::lanes) + " is missing.");

    // parse lane offsets
    QDomElement laneOffsetElement;
    if (SimulationCommon::GetFirstChildElement(roadLanesElement, TAG::laneOffset, laneOffsetElement))
    {
        while (!laneOffsetElement.isNull())
        {
            double laneOffsetS, laneOffsetA, laneOffsetB, laneOffsetC, laneOffsetD;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::s, laneOffsetS, 0.0),
                          "Could not parse road lanes. LaneOffset tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::a, laneOffsetA, 0.0),
                          "Could not parse road lanes. LaneOffset tag requires a " + std::string(ATTRIBUTE::a) + " attribute.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::b, laneOffsetB, 0.0),
                         "Could not parse road lanes. LaneOffset tag requires a " + std::string(ATTRIBUTE::b) + " attribute.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::c, laneOffsetC, 0.0),
                          "Could not parse road lanes. LaneOffset tag requires a " + std::string(ATTRIBUTE::c) + " attribute.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::d, laneOffsetD, 0.0),
                          "Could not parse road lanes. LaneOffset tag requires a " + std::string(ATTRIBUTE::d) + " attribute.");

            road->AddLaneOffset(laneOffsetS,
                                laneOffsetA,
                                laneOffsetB,
                                laneOffsetC,
                                laneOffsetD);

            laneOffsetElement = laneOffsetElement.nextSiblingElement(TAG::laneOffset);
        } // laneOffset loop
    } // if laneOffsets exist

    // parse lane sections
    QDomElement roadLaneSectionElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadLanesElement, TAG::laneSection, roadLaneSectionElement),
                  "Could not parse road lanes. Tag " + std::string(TAG::laneSection) + " is missing.");
    while (!roadLaneSectionElement.isNull())
    {
        double roadLaneSectionStart;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneSectionElement, ATTRIBUTE::s, roadLaneSectionStart),
                      "Could not parse road lanes. LaneSection tag requires a " + std::string(ATTRIBUTE::s) + " attribute.");

        // add OpenDrive lane section in ascending order
        RoadLaneSectionInterface* laneSection = road->AddRoadLaneSection(roadLaneSectionStart);
        ThrowIfFalse(laneSection != nullptr, "laneSection is null");

        LOG_INTERN(LogLevel::DebugCore) << "lane section (s=" << roadLaneSectionStart << ")";

        // left lanes
        QDomElement roadLaneSectionSideElement;

        if (SimulationCommon::GetFirstChildElement(roadLaneSectionElement, TAG::left, roadLaneSectionSideElement))
        {
            ParseLanes(roadLaneSectionSideElement, laneSection);
        }

        // center lanes
        if (SimulationCommon::GetFirstChildElement(roadLaneSectionElement, TAG::center, roadLaneSectionSideElement))
        {
            ParseLanes(roadLaneSectionSideElement, laneSection);
        }

        // right lanes
        if (SimulationCommon::GetFirstChildElement(roadLaneSectionElement, TAG::right, roadLaneSectionSideElement))
        {
            ParseLanes(roadLaneSectionSideElement, laneSection);
        }
    roadLaneSectionElement = roadLaneSectionElement.nextSiblingElement(TAG::laneSection);
    } // road lane section loop
}

void SceneryImporter::ParseJunctionConnections(QDomElement& junctionElement, JunctionInterface* junction)
{
    QDomElement connectionElement;
    if (SimulationCommon::GetFirstChildElement(junctionElement, TAG::connection, connectionElement))
    {
        while (!connectionElement.isNull())
        {
            std::string id;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::id, id),
                          "Could not parse junction connections. Connection tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");
            std::string incomingRoad;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::incomingRoad, incomingRoad),
                          "Could not parse junction connections. Connection tag requires a " + std::string(ATTRIBUTE::incomingRoad) + " attribute.");
            std::string connectingRoad;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::connectingRoad, connectingRoad),
                          "Could not parse junction connections. Connection tag requires a " + std::string(ATTRIBUTE::connectingRoad) + " attribute.");
            std::string contactPoint;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::contactPoint, contactPoint),
                          "Could not parse junction connections. Connection tag requires a " + std::string(ATTRIBUTE::contactPoint) + " attribute.");

            ContactPointType roadLinkContactPoint = ContactPointType::Undefined;


            if (0 == contactPoint.compare("start"))
            {
                roadLinkContactPoint = ContactPointType::Start;
            }
            else
                if (0 == contactPoint.compare("end"))
                {
                    roadLinkContactPoint = ContactPointType::End;
                }


            ConnectionInterface* connection = junction->AddConnection(id, incomingRoad, connectingRoad, roadLinkContactPoint);

            ParseJunctionConnectionLinks(connectionElement, connection);

            connectionElement = connectionElement.nextSiblingElement(TAG::connection);
        }
    }
}

void SceneryImporter::ParseJunctionConnectionLinks(QDomElement& connectionElement, ConnectionInterface* connection)
{
    QDomElement linkElement;
    if (SimulationCommon::GetFirstChildElement(connectionElement, TAG::laneLink, linkElement))
    {
        while (!linkElement.isNull())
        {
            int from;
            ThrowIfFalse(SimulationCommon::ParseAttributeInt(linkElement, ATTRIBUTE::from, from),
                          "Could not parse junction connection links. Link tag requires a " + std::string(ATTRIBUTE::from) + " attribute.");

            int to;
            ThrowIfFalse(SimulationCommon::ParseAttributeInt(linkElement, ATTRIBUTE::to, to),
                          "Could not parse junction connection links. Link tag requires a " + std::string(ATTRIBUTE::to) + " attribute.");
            connection->AddLink(from, to);

            linkElement = linkElement.nextSiblingElement(TAG::laneLink);
        }
    }
}

void SceneryImporter::ParseJunctionPriorities(QDomElement &junctionElement, JunctionInterface *junction)
{
    QDomElement priorityElement;
    if (SimulationCommon::GetFirstChildElement(junctionElement, TAG::priority, priorityElement))
    {
        while (!priorityElement.isNull())
        {
            Priority priority;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(priorityElement, ATTRIBUTE::high, priority.high),
                          "Could not parse junction priorities. Priority tag requires a " + std::string(ATTRIBUTE::high) + " attribute.");
            ThrowIfFalse(SimulationCommon::ParseAttributeString(priorityElement, ATTRIBUTE::low, priority.low),
                          "Could not parse junction priorities. Priority tag requires a " + std::string(ATTRIBUTE::low) + " attribute.");

            junction->AddPriority(priority);

            priorityElement = priorityElement.nextSiblingElement(TAG::priority);
        }
    }
}

void SceneryImporter::ParseRoads(QDomElement& documentRoot,
                                 Scenery* scenery)
{
    QDomElement roadElement;
    if (SimulationCommon::GetFirstChildElement(documentRoot, TAG::road, roadElement))
    {
        while (!roadElement.isNull())
        {
            // road id
            std::string id;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(roadElement, ATTRIBUTE::id, id),
                          "Could not parse roads. Road tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");

            RoadInterface* road = scenery->AddRoad(id);
            ThrowIfFalse(road != nullptr, "road is null");

            std::string junctionId;
            if (!SimulationCommon::ParseAttributeString(roadElement, ATTRIBUTE::junction, junctionId))
            {
                junctionId = "-1";
            }
            road->SetJunctionId(junctionId);


            LOG_INTERN(LogLevel::DebugCore) << "road: id: " << id;

            ParseGeometries(roadElement, road);

            ParseElevationProfile(roadElement, road);

            ParseRoadLinks(roadElement, road);

            ParseRoadLanes(roadElement, road); // parsing laneOffset is included here

            ParseObjects(roadElement, road);

            ParseSignals(roadElement, road);

            ParseRoadTypes(roadElement, road);

            roadElement = roadElement.nextSiblingElement(TAG::road);
        } // road loop
    } // if roads exist
}

void SceneryImporter::ParseJunctions(QDomElement& documentRoot, Scenery* scenery)
{

    QDomElement junctionElement;
    if (SimulationCommon::GetFirstChildElement(documentRoot, TAG::junction, junctionElement))
    {
        while (!junctionElement.isNull())
        {
            std::string id;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(junctionElement, ATTRIBUTE::id, id),
                          "Could not parse junctions. Junction tag requires a " + std::string(ATTRIBUTE::id) + " attribute.");

            JunctionInterface* junction = scenery->AddJunction(id);

            ParseJunctionConnections(junctionElement, junction);
            ParseJunctionPriorities(junctionElement, junction);

            junctionElement = junctionElement.nextSiblingElement(TAG::junction);
        }
    }
}

//-----------------------------------------------------------------------------
//! Imports a scenery from a given file
//!
//!
//! @param[in]  filename            DOM element containing e.g. OPENDrive road
//! @param[out] globalObjects       Target container for the scenery data
//!
//! @return                         False if an error occurred, true otherwise
//-----------------------------------------------------------------------------
bool SceneryImporter::Import(const std::string& filename,
                             Scenery* scenery)
{
    try
    {
        std::locale::global(std::locale("C"));

        QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
        ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly),
                     "an error occurred during scenery import");

        QByteArray xmlData(xmlFile.readAll());
        QDomDocument document;
        QString errorMsg;
        int errorLine;
        ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine),
                     "Invalid xml file format of file " + filename + " in line " + std::to_string(errorLine) + " : " + errorMsg.toStdString());

        QDomElement documentRoot = document.documentElement();
        if (documentRoot.isNull())
        {
            return false;
        }

        // parse junctions
        ParseJunctions(documentRoot, scenery);

        // parse roads
        ParseRoads(documentRoot, scenery);

        return true;
    }
    catch (std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Scenery import failed: " + std::string(e.what());
        return false;
    }
}

} // namespace Importer


