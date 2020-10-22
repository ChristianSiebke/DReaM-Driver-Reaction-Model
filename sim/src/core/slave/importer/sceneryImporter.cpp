/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "common/opMath.h"
#include <algorithm>
#include <utility>
#include <limits>
#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include <QFile>

#include "scenery.h"
#include "sceneryImporter.h"
#include "importer/importerCommon.h"
#include "common/commonTools.h"
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


template <typename T>
bool assignIfMatching(const std::string &element, T &enumeration, const std::string &match, const T &value)
{
    if (element == match)
    {
        enumeration = value;
        return true;
    }
    return false;
}

bool ParseType(const std::string &element, RoadElementOrientation &orientation)
{
    return
            assignIfMatching(element, orientation, "none", RoadElementOrientation::both) ||
            assignIfMatching(element, orientation, "+",    RoadElementOrientation::positive) ||
            assignIfMatching(element, orientation, "-",    RoadElementOrientation::negative);
}

bool ParseType(const std::string &element, RoadObjectType &objectType)
{
    return
            assignIfMatching(element, objectType, "barrier", RoadObjectType::barrier) ||
            assignIfMatching(element, objectType, "obstacle", RoadObjectType::obstacle) ||
            assignIfMatching(element, objectType, "car", RoadObjectType::car) ||
            assignIfMatching(element, objectType, "truck", RoadObjectType::truck) ||
            assignIfMatching(element, objectType, "van", RoadObjectType::van) ||
            assignIfMatching(element, objectType, "bus", RoadObjectType::bus) ||
            assignIfMatching(element, objectType, "trailer", RoadObjectType::trailer) ||
            assignIfMatching(element, objectType, "bike", RoadObjectType::bike) ||
            assignIfMatching(element, objectType, "motorbike", RoadObjectType::motorbike) ||
            assignIfMatching(element, objectType, "tram", RoadObjectType::tram) ||
            assignIfMatching(element, objectType, "train", RoadObjectType::train) ||
            assignIfMatching(element, objectType, "pedestrian", RoadObjectType::pedestrian)||
            assignIfMatching(element, objectType, "pole", RoadObjectType::pole) ||
            assignIfMatching(element, objectType, "tree", RoadObjectType::tree) ||
            assignIfMatching(element, objectType, "vegetation", RoadObjectType::vegetation) ||
            assignIfMatching(element, objectType, "building", RoadObjectType::building) ||
            assignIfMatching(element, objectType, "parkingSpace", RoadObjectType::parkingSpace) ||
            assignIfMatching(element, objectType, "wind", RoadObjectType::wind)  ||
            assignIfMatching(element, objectType, "patch", RoadObjectType::patch) ||
            assignIfMatching(element, objectType, "GuardRail", RoadObjectType::guardRail) ||
            assignIfMatching(element, objectType, "RoadSideMarkerPost", RoadObjectType::roadSideMarkerPost);
}

template <typename T>
bool ParseAttributeType(QDomElement element, const std::string &attributeName, T &result)
{
    if (!element.hasAttribute(QString::fromStdString(attributeName)))
    {
        return false;
    }

    QDomAttr attribute = element.attributeNode(QString::fromStdString(attributeName));
    if (attribute.isNull())
    {
        return false;
    }

    return ParseType(attribute.value().toStdString(), result);
}

void SceneryImporter::ParseLanes(QDomElement& rootElement,
                                 RoadLaneSectionInterface* laneSection)
{
    QDomElement roadLaneElement;
    SimulationCommon::GetFirstChildElement(rootElement, TAG::lane, roadLaneElement);

    while (!roadLaneElement.isNull())
    {
        int roadLaneId;
        ThrowIfFalse(SimulationCommon::ParseAttributeInt(roadLaneElement, ATTRIBUTE::id, roadLaneId),
                     roadLaneElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");

        std::string roadLaneTypeStr;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLaneElement, ATTRIBUTE::type, roadLaneTypeStr),
                     roadLaneElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

        RoadLaneType roadLaneType = roadLaneTypeConversionMap.at(roadLaneTypeStr);

        RoadLaneInterface* roadLane = laneSection->AddRoadLane(roadLaneId,
                                      roadLaneType);
        ThrowIfFalse(roadLane, roadLaneElement, "Id must be unique");
        LOG_INTERN(LogLevel::DebugCore) << "lane: type " << static_cast<int>(roadLaneType) << ", id=" << roadLaneId;

        if (0 != roadLaneId) // skip center lanes
        {
            QDomElement widthOrBorderElement;
            // at least one width element necessary
            if(SimulationCommon::GetFirstChildElement(roadLaneElement, TAG::width, widthOrBorderElement))
            {
                while (!widthOrBorderElement.isNull())
                {
                    double sOffset, a, b, c, d;
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::sOffset, sOffset),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::sOffset) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::a, a),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::a) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::b, b),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::b) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::c, c),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::c) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::d, d),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::d) + " is missing.");

                    roadLane->AddWidth(sOffset,
                                       a,
                                       b,
                                       c,
                                       d);

                    widthOrBorderElement = widthOrBorderElement.nextSiblingElement(TAG::width);
                }
            }
            else if (SimulationCommon::GetFirstChildElement(roadLaneElement, TAG::border, widthOrBorderElement))
            {
                while (!widthOrBorderElement.isNull())
                {
                    double sOffset, a, b, c, d;
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::sOffset, sOffset),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::sOffset) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::a, a),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::a) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::b, b),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::b) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::c, c),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::c) + " is missing.");
                    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(widthOrBorderElement, ATTRIBUTE::d, d),
                                 widthOrBorderElement, "Attribute " + std::string(ATTRIBUTE::d) + " is missing.");

                    roadLane->AddBorder(sOffset,
                                       a,
                                       b,
                                       c,
                                       d);

                    widthOrBorderElement = widthOrBorderElement.nextSiblingElement(TAG::border);
                }
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
                             roadLaneLinkItemElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");
                roadLane->AddSuccessor(roadLaneLinkItemId);
            }

            if (SimulationCommon::GetFirstChildElement(roadLaneLinkElement, TAG::predecessor, roadLaneLinkItemElement))
            {
                ThrowIfFalse(SimulationCommon::ParseAttributeInt(roadLaneLinkItemElement, ATTRIBUTE::id, roadLaneLinkItemId),
                             roadLaneLinkItemElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");
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
                     roadLaneRoadMarkElement, "Attribute " + std::string(ATTRIBUTE::sOffset) + " is missing.");

        std::string roadMarkTypeStr;
        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLaneRoadMarkElement, ATTRIBUTE::type, roadMarkTypeStr, "none"),
                     roadLaneRoadMarkElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
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
                     roadLaneRoadMarkElement, "Attribute " + std::string(ATTRIBUTE::color) + " is missing.");
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
                     roadLaneRoadMarkElement, "Attribute " + std::string(ATTRIBUTE::weight) + " is missing.");
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
                 roadElement, "Tag " + std::string(TAG::planView) + " is missing.");

    QDomElement roadGeometryHeaderElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(roadPlanView, TAG::geometry, roadGeometryHeaderElement),
                 roadPlanView, "Tag " + std::string(TAG::geometry) + " is missing.");
    while (!roadGeometryHeaderElement.isNull())
    {
        double roadGeometryS;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::s, roadGeometryS),
                     roadGeometryHeaderElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");
        roadGeometryS = CommonHelper::roundDoubleWithDecimals(roadGeometryS, 3);

        double roadGeometryX;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::x, roadGeometryX),
                     roadGeometryHeaderElement, "Attribute " + std::string(ATTRIBUTE::x) + " is missing.");
        roadGeometryX = CommonHelper::roundDoubleWithDecimals(roadGeometryX, 3);

        double roadGeometryY;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::y, roadGeometryY),
                     roadGeometryHeaderElement, "Attribute " + std::string(ATTRIBUTE::y) + " is missing.");
        roadGeometryY = CommonHelper::roundDoubleWithDecimals(roadGeometryY, 3);

        double roadGeometryHdg;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::hdg, roadGeometryHdg),
                     roadGeometryHeaderElement, "Attribute " + std::string(ATTRIBUTE::hdg) + " is missing.");
        roadGeometryHdg = CommonHelper::roundDoubleWithDecimals(roadGeometryHdg, 6);

        double roadGeometryLength;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryHeaderElement, ATTRIBUTE::length, roadGeometryLength),
                     roadGeometryHeaderElement, "Attribute " + std::string(ATTRIBUTE::length) + " is missing.");
        roadGeometryLength = CommonHelper::roundDoubleWithDecimals(roadGeometryLength, 3);

        QDomElement roadGeometryElement;
        if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::line, roadGeometryElement))
        {
           ThrowIfFalse(road->AddGeometryLine(roadGeometryS,
                                             roadGeometryX,
                                             roadGeometryY,
                                             roadGeometryHdg,
                                             roadGeometryLength),
                        roadGeometryElement,
                        "Unable to add geometry line.");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::arc, roadGeometryElement))
        {
            double roadGeometryCurvature;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::curvature, roadGeometryCurvature),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::curvature) + " is missing.");
            ThrowIfFalse(road->AddGeometryArc(roadGeometryS,
                                            roadGeometryX,
                                            roadGeometryY,
                                            roadGeometryHdg,
                                            roadGeometryLength,
                                            roadGeometryCurvature),
                         roadGeometryElement,
                          "Unable to add geometry arc");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::spiral, roadGeometryElement))
        {
            double roadGeometryCurvStart;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::curvStart, roadGeometryCurvStart),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::curvStart) + " is missing.");

            double roadGeometryCurvEnd;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::curvEnd, roadGeometryCurvEnd),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::curvEnd) + " is missing.");

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
                         roadGeometryElement,
                          "Unable to add geometry spiral.");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::poly3, roadGeometryElement))
        {
            double roadGeometryA;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::a, roadGeometryA),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::a) + " is missing.");

            double roadGeometryB;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::b, roadGeometryB),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::b) + " is missing.");

            double roadGeometryC;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::c, roadGeometryC),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::c) + " is missing.");

            double roadGeometryD;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::d, roadGeometryD),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::d) + " is missing.");
            ThrowIfFalse(road->AddGeometryPoly3(roadGeometryS,
                                              roadGeometryX,
                                              roadGeometryY,
                                              roadGeometryHdg,
                                              roadGeometryLength,
                                              roadGeometryA,
                                              roadGeometryB,
                                              roadGeometryC,
                                              roadGeometryD),
                         roadGeometryElement,
                          "Unable to add geometry poly3.");
        }
        else if (SimulationCommon::GetFirstChildElement(roadGeometryHeaderElement, TAG::paramPoly3, roadGeometryElement))
        {
            double roadGeometryaU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::aU, roadGeometryaU),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::aU) + " is missing.");

            double roadGeometrybU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::bU, roadGeometrybU),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::bU) + " is missing.");

            double roadGeometrycU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::cU, roadGeometrycU),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::cU) + " is missing.");

            double roadGeometrydU;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::dU, roadGeometrydU),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::dU) + " is missing.");

            double roadGeometryaV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::aV, roadGeometryaV),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::aV) + " is missing.");

            double roadGeometrybV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::bV, roadGeometrybV),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::bV) + " is missing.");

            double roadGeometrycV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::cV, roadGeometrycV),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::cV) + " is missing.");

            double roadGeometrydV;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadGeometryElement, ATTRIBUTE::dV, roadGeometrydV),
                         roadGeometryElement, "Attribute " + std::string(ATTRIBUTE::dV) + " is missing.");

            ParamPoly3Parameters parameters;
            parameters = {roadGeometryaU, roadGeometrybU, roadGeometrycU, roadGeometrydU, roadGeometryaV, roadGeometrybV, roadGeometrycV, roadGeometrydV};
            ThrowIfFalse(road->AddGeometryParamPoly3(roadGeometryS,
                                                   roadGeometryX,
                                                   roadGeometryY,
                                                   roadGeometryHdg,
                                                   roadGeometryLength,
                                                   parameters),
                         roadGeometryElement,
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
                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement, ATTRIBUTE::s, elevationS),
                             elevationElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement, ATTRIBUTE::a, elevationA),
                             elevationElement, "Attribute " + std::string(ATTRIBUTE::a) + " is missing.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement, ATTRIBUTE::b, elevationB),
                             elevationElement, "Attribute " + std::string(ATTRIBUTE::b) + " is missing.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement, ATTRIBUTE::c, elevationC),
                             elevationElement, "Attribute " + std::string(ATTRIBUTE::c) + " is missing.");

                ThrowIfFalse(SimulationCommon::ParseAttributeDouble(elevationElement, ATTRIBUTE::d, elevationD),
                             elevationElement, "Attribute " + std::string(ATTRIBUTE::d) + " is missing.");

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
                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::elementType, roadLinkItem_ElementTypeAttribute),
                                 roadLinkItemElement, "Attribute " + std::string(ATTRIBUTE::elementType) + " is missing.");

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
                                 roadLinkItemElement, "Attribute " + std::string(ATTRIBUTE::elementId) + " is missing.");

                    if (RoadLinkElementType::Road == roadLinkElementType)
                    {
                        std::string roadLinkItem_ContactPointAttribute;
                        ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::contactPoint, roadLinkItem_ContactPointAttribute),
                                     roadLinkItemElement, "Attribute " + std::string(ATTRIBUTE::contactPoint) + " is missing.");

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
                                 roadLinkItemElement, "Attribute " + std::string(ATTRIBUTE::side) + " is missing.");

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
                                 roadLinkItemElement, "Attribute " + std::string(ATTRIBUTE::elementId) + " is missing.");

                    std::string roadLinkItem_DirectionAttribute;
                    ThrowIfFalse(SimulationCommon::ParseAttributeString(roadLinkItemElement, ATTRIBUTE::direction, roadLinkItem_DirectionAttribute),
                                 roadLinkItemElement, "Attribute " + std::string(ATTRIBUTE::direction) + " is missing.");

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
                             roadLinkItemElement,
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
                 signal.height >= 0 &&
                 signal.width >= 0),
            "Invalid road signal boundaries.");
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
                             signalElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");
                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::t,           signal.t),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::t) + " is missing.");
                ThrowIfFalse(ParseAttributeDouble(signalElement, ATTRIBUTE::zOffset,     signal.zOffset),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::zOffset) + " is missing.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::id,          signal.id),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::dynamic,     signal.dynamic, "no"),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::dynamic) + " is missing.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::orientation, signal.orientation),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::orientation) + " is missing.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::type,        signal.type),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");
                ThrowIfFalse(ParseAttributeString(signalElement, ATTRIBUTE::subtype,     signal.subtype),
                             signalElement, "Attribute " + std::string(ATTRIBUTE::subtype) + " is missing.");
                // optional
                std::string signalUnit;
                ParseAttributeDouble(signalElement, ATTRIBUTE::value, signal.value);
                ParseAttributeString(signalElement, ATTRIBUTE::unit,  signalUnit);
                ParseSignalUnit(signalUnit, signal.unit);
                ParseAttributeDouble(signalElement, ATTRIBUTE::hOffset, signal.hOffset);
                ParseAttributeDouble(signalElement, ATTRIBUTE::pitch,   signal.pitch);
                ParseAttributeString(signalElement, ATTRIBUTE::name,    signal.name);
                ParseAttributeDouble(signalElement, ATTRIBUTE::height,  signal.height);
                ParseAttributeDouble(signalElement, ATTRIBUTE::width,   signal.width);
                ParseAttributeString(signalElement, ATTRIBUTE::text,    signal.text);
                ParseAttributeString(signalElement, ATTRIBUTE::country, signal.country);

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
                                     dependencyElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");
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
                 objectElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing or unknown.");
    ParseAttributeString(objectElement, ATTRIBUTE::name, object.name);
    ThrowIfFalse(ParseAttributeString(objectElement, ATTRIBUTE::id, object.id),
                 objectElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::s, object.s),
                 objectElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::t, object.t),
                 objectElement, "Attribute " + std::string(ATTRIBUTE::t) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(objectElement, ATTRIBUTE::zOffset, object.zOffset),
                 objectElement, "Attribute " + std::string(ATTRIBUTE::zOffset) + " is missing.");
    ParseAttributeDouble(objectElement, ATTRIBUTE::validLength, object.validLength);
    ParseAttributeType(objectElement, ATTRIBUTE::orientation, object.orientation);
    ParseAttributeDouble(objectElement, ATTRIBUTE::width, object.width);
    ParseAttributeDouble(objectElement, ATTRIBUTE::length, object.length);
    ParseAttributeDouble(objectElement, ATTRIBUTE::height, object.height);
    ParseAttributeDouble(objectElement, ATTRIBUTE::hdg, object.hdg);
    ParseAttributeDouble(objectElement, ATTRIBUTE::pitch, object.pitch);
    ParseAttributeDouble(objectElement, ATTRIBUTE::roll, object.roll);
    ParseAttributeDouble(objectElement, "radius", object.radius);

    ThrowIfFalse(object.checkStandardCompliance(), objectElement, "limits of object are not valid for openDrive standard");

    ThrowIfFalse(object.checkSimulatorCompliance(), objectElement, "Limits of object are not valid for the simulation. The Object will be ignored.");

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
    ThrowIfFalse(!checkStartIsSet && !checkEndIsSet, repeatElement, "Missing intervall parameter in scenery import");
    return;
}

void SceneryImporter::ParseRepeat(QDomElement& repeatElement, RoadObjectSpecification object,
                                  std::list<RoadObjectSpecification>& objectRepitions)
{
    using namespace SimulationCommon;
    ObjectRepeat repeat;

    ThrowIfFalse(ParseAttributeDouble(repeatElement, ATTRIBUTE::s,            repeat.s),
                 repeatElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(repeatElement, ATTRIBUTE::length,       repeat.length),
                 repeatElement, "Attribute " + std::string(ATTRIBUTE::length) + " is missing.");
    ThrowIfFalse(ParseAttributeDouble(repeatElement, ATTRIBUTE::distance,     repeat.distance),
                 repeatElement, "Attribute " + std::string(ATTRIBUTE::distance) + " is missing.");

    ParseOptionalInterval(repeatElement, "tStart", "tEnd", repeat.t);
    ParseOptionalInterval(repeatElement, "widthStart", "widthEnd", repeat.width);
    ParseOptionalInterval(repeatElement, "heightStart", "heightEnd", repeat.height);
    ParseOptionalInterval(repeatElement, "zOffsetStart", "zOffsetEnd", repeat.zOffset);

    ThrowIfFalse(repeat.checkLimits(), repeatElement, "Invalid limits.");

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
                     validityElement, "Attribute " + std::string(ATTRIBUTE::fromLane) + " is missing.");
        ThrowIfFalse(ParseAttributeInt(validityElement, ATTRIBUTE::toLane,   toLane),
                     validityElement, "Attribute " + std::string(ATTRIBUTE::toLane) + " is missing.");

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
                         typeElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");
            std::string roadTypeStr;
            ThrowIfFalse(ParseAttributeString(typeElement, ATTRIBUTE::type,           roadTypeStr),
                         typeElement, "Attribute " + std::string(ATTRIBUTE::type) + " is missing.");

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
                 roadElement, "Tag + " + std::string(TAG::lanes) + " is missing.");

    // parse lane offsets
    QDomElement laneOffsetElement;
    if (SimulationCommon::GetFirstChildElement(roadLanesElement, TAG::laneOffset, laneOffsetElement))
    {
        while (!laneOffsetElement.isNull())
        {
            double laneOffsetS, laneOffsetA, laneOffsetB, laneOffsetC, laneOffsetD;
            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::s, laneOffsetS, 0.0),
                         laneOffsetElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::a, laneOffsetA, 0.0),
                         laneOffsetElement, "Attribute " + std::string(ATTRIBUTE::a) + " is missing.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::b, laneOffsetB, 0.0),
                         laneOffsetElement, "Attribute " + std::string(ATTRIBUTE::b) + " is missing.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::c, laneOffsetC, 0.0),
                         laneOffsetElement, "Attribute " + std::string(ATTRIBUTE::c) + " is missing.");

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(laneOffsetElement, ATTRIBUTE::d, laneOffsetD, 0.0),
                         laneOffsetElement, "Attribute " + std::string(ATTRIBUTE::d) + " is missing.");

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
                 roadLanesElement, "Tag " + std::string(TAG::laneSection) + " is missing.");
    while (!roadLaneSectionElement.isNull())
    {
        double roadLaneSectionStart;
        ThrowIfFalse(SimulationCommon::ParseAttributeDouble(roadLaneSectionElement, ATTRIBUTE::s, roadLaneSectionStart),
                     roadLaneSectionElement, "Attribute " + std::string(ATTRIBUTE::s) + " is missing.");

        // add OpenDrive lane section in ascending order
        RoadLaneSectionInterface* laneSection = road->AddRoadLaneSection(roadLaneSectionStart);
        ThrowIfFalse(laneSection != nullptr, roadLanesElement, "Could not add Section");

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
                         connectionElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");
            std::string incomingRoad;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::incomingRoad, incomingRoad),
                         connectionElement, "Attribute " + std::string(ATTRIBUTE::incomingRoad) + " is missing.");
            std::string connectingRoad;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::connectingRoad, connectingRoad),
                         connectionElement, "Attribute " + std::string(ATTRIBUTE::connectingRoad) + " is missing.");
            std::string contactPoint;
            ThrowIfFalse(SimulationCommon::ParseAttributeString(connectionElement, ATTRIBUTE::contactPoint, contactPoint),
                         connectionElement, "Attribute " + std::string(ATTRIBUTE::contactPoint) + " is missing.");

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
                         linkElement, "Attribute " + std::string(ATTRIBUTE::from) + " is missing.");

            int to;
            ThrowIfFalse(SimulationCommon::ParseAttributeInt(linkElement, ATTRIBUTE::to, to),
                         linkElement, "Attribute " + std::string(ATTRIBUTE::to) + " is missing.");
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
                         priorityElement, "Attribute " + std::string(ATTRIBUTE::high) + " is missing.");
            ThrowIfFalse(SimulationCommon::ParseAttributeString(priorityElement, ATTRIBUTE::low, priority.low),
                         priorityElement, "Attribute " + std::string(ATTRIBUTE::low) + " is missing.");

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
                         roadElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");

            RoadInterface* road = scenery->AddRoad(id);
            ThrowIfFalse(road != nullptr, roadElement, "Could not add Road");

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
                         junctionElement, "Attribute " + std::string(ATTRIBUTE::id) + " is missing.");

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
        QString errorMsg {};
        int errorLine {};
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


