/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include "importerLoggingHelper.h"
#include "vehicleModelsImporter.h"

namespace TAG = openpass::importer::xml::vehicleModelsImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::vehicleModelsImporter::attribute;

namespace Importer {

bool VehicleModelsImporter::Import(const std::string& vehicleCatalogPath,
                                   const std::string& pedestrianCatalogPath,
                                   Configuration::VehicleModels& vehicleModels)
{
    try
    {
        QDomElement catalogElement;

        if (!vehicleCatalogPath.empty())
        {
            ImportCatalog(vehicleCatalogPath, catalogElement);
            ImportVehicleCatalog(catalogElement, vehicleModels);
        }

        if (!pedestrianCatalogPath.empty())
        {
            ImportCatalog(pedestrianCatalogPath, catalogElement);
            ImportPedestrianCatalog(catalogElement, vehicleModels);
        }

        return true;
    }
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "VehicleModels import failed: " + std::string(e.what());
        return false;
    }
}

void VehicleModelsImporter::ImportVehicleCatalog(QDomElement& catalogElement,
        Configuration::VehicleModels& vehicleModels)
{
    QDomElement vehicleElement;

    if (SimulationCommon::GetFirstChildElement(catalogElement, TAG::vehicle, vehicleElement))
    {
        //iterates over all models
        while (!vehicleElement.isNull())
        {
            ImportVehicleModel(vehicleElement, vehicleModels.GetVehicleModelMap());
            vehicleElement = vehicleElement.nextSiblingElement(std::string(TAG::vehicle).c_str());
        }
    }
}

void VehicleModelsImporter::ImportPedestrianCatalog(QDomElement& catalogElement,
        Configuration::VehicleModels& vehicleModels)
{
    QDomElement pedestrianElement;

    if (SimulationCommon::GetFirstChildElement(catalogElement, TAG::pedestrian, pedestrianElement))
    {
        //iterates over all models
        while (!pedestrianElement.isNull())
        {
            ImportPedestrianModel(pedestrianElement, vehicleModels.GetVehicleModelMap());
            pedestrianElement = pedestrianElement.nextSiblingElement(std::string(TAG::pedestrian).c_str());
        }
    }
}

void VehicleModelsImporter::ImportCatalog(const std::string& catalogPath, QDomElement& catalogElement)
{
    LOG_INTERN(LogLevel::Info) << "Importing catalog from " << catalogPath;

    QFile xmlFile(catalogPath.c_str()); // automatic object will be closed on destruction
    ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "Could not open " + catalogPath);

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg {};
    int errorLine {};
    ThrowIfFalse(document.setContent(xmlData, &errorMsg, &errorLine),
                 "Invalid xml file format of file " + catalogPath + " in line " + std::to_string(errorLine) + " : " + errorMsg.toStdString());

    const QDomElement documentRoot = document.documentElement();

    ThrowIfFalse(!documentRoot.isNull(), "Root xml element not found in " + catalogPath);

    ThrowIfFalse(SimulationCommon::GetFirstChildElement(documentRoot, TAG::catalog, catalogElement),
                  "Could not import catalog. Tag " + std::string(TAG::catalog) + " is missing.");
}

void VehicleModelsImporter::CheckModelParameters(const VehicleModelParameters& model)
{
    assert(model.length > 0);
    assert(model.width > 0);
    assert(model.height > 0);
    assert(model.trackwidth > 0);
    assert(model.wheelbase > 0);
    assert(model.maxVelocity > 0);
    assert(model.maxCurvature > 0);
    assert(model.vehicleType != AgentVehicleType::Undefined);
}

void VehicleModelsImporter::ImportVehicleModelAxles(QDomElement& vehicleElement,
        VehicleModelParameters& modelParameters)
{
    QDomElement axlesElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(vehicleElement, TAG::axles, axlesElement),
                 vehicleElement, "Tag " + std::string(TAG::axles) + " is missing.");

    QDomElement frontAxleElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(axlesElement, TAG::front, frontAxleElement),
                 axlesElement, "Tag " + std::string(TAG::front) + " is missing.");

    QDomElement rearAxleElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(axlesElement, TAG::rear, rearAxleElement),
                 axlesElement, "Tag " + std::string(TAG::rear) + " is missing.");

    VehicleAxle frontAxleParameters;
    VehicleAxle rearAxleParameters;

    ImportVehicleModelAxle(frontAxleElement, frontAxleParameters);
    ImportVehicleModelAxle(rearAxleElement, rearAxleParameters);
    ValidateAxles(frontAxleParameters, rearAxleParameters);
    AssignModelParameters(frontAxleParameters, rearAxleParameters, modelParameters);
}

void VehicleModelsImporter::AssignModelParameters(const VehicleAxle& frontAxle, const VehicleAxle& rearAxle,
        VehicleModelParameters& modelParameters)
{
    modelParameters.staticWheelRadius = rearAxle.wheelDiameter / 2.0;
    modelParameters.distanceReferencePointToFrontAxle = frontAxle.positionX;
    modelParameters.wheelbase = std::abs(frontAxle.positionX - rearAxle.positionX);
    modelParameters.trackwidth = rearAxle.trackWidth;
    modelParameters.maxCurvature = std::sin(frontAxle.maxSteering) / modelParameters.wheelbase;
    modelParameters.maximumSteeringWheelAngleAmplitude = frontAxle.maxSteering * modelParameters.steeringRatio * 180.0 /
            M_PI;
}

void VehicleModelsImporter::ImportVehicleModelAxle(QDomElement& axleElement, VehicleAxle& axleParameters)
{
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(axleElement, ATTRIBUTE::wheelDiameter, axleParameters.wheelDiameter),
                 axleElement, "Attribute " + std::string(ATTRIBUTE::wheelDiameter) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(axleElement, ATTRIBUTE::positionX, axleParameters.positionX),
                 axleElement, "Attribute " + std::string(ATTRIBUTE::positionX) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(axleElement, ATTRIBUTE::trackWidth, axleParameters.trackWidth),
                 axleElement, "Attribute " + std::string(ATTRIBUTE::trackWidth) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(axleElement, ATTRIBUTE::maxSteering, axleParameters.maxSteering),
                 axleElement, "Attribute " + std::string(ATTRIBUTE::maxSteering) + " is missing.");
}

void VehicleModelsImporter::ValidateAxles(const VehicleAxle& frontAxle, const VehicleAxle& rearAxle)
{
    if (std::abs(frontAxle.wheelDiameter - rearAxle.wheelDiameter) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) <<
                                      "Different wheel diameters for front and rear axle not supported. Using rear axle value.";
    }

    ThrowIfFalse(rearAxle.positionX == 0.0, "Reference point not on rear axle.");

    if (rearAxle.positionX > frontAxle.positionX)
    {
        LOG_INTERN(LogLevel::Warning) << "Front axle is located behind rear axle.";
    }

    if (std::abs(frontAxle.trackWidth - rearAxle.trackWidth) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) << "Different track width for front and rear axle not supported. Using rear axle value.";
    }

    ThrowIfFalse(rearAxle.maxSteering == 0.0, "Rear axle steering not supported.");
}

void VehicleModelsImporter::ImportModelBoundingBox(QDomElement& modelElement, VehicleModelParameters& modelParameters)
{
    QDomElement boundingBoxElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(modelElement, TAG::boundingBox, boundingBoxElement),
                 modelElement, "Tag " + std::string(TAG::boundingBox) + " is missing.");

    QDomElement boundingBoxCenterElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(boundingBoxElement, TAG::center, boundingBoxCenterElement),
                 boundingBoxElement, "Tag " + std::string(TAG::center) + " is missing.");

    QDomElement boundingBoxDimensionElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(boundingBoxElement, TAG::dimension, boundingBoxDimensionElement),
                 boundingBoxElement, "Tag " + std::string(TAG::dimension) + " is missing.");

    double bbCenterX, bbCenterY, bbCenterZ;
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(boundingBoxCenterElement, ATTRIBUTE::x, bbCenterX),
                 boundingBoxCenterElement, "Attribute " + std::string(ATTRIBUTE::x) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(boundingBoxCenterElement, ATTRIBUTE::y, bbCenterY),
                 boundingBoxCenterElement, "Attribute " + std::string(ATTRIBUTE::y) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(boundingBoxCenterElement, ATTRIBUTE::z, bbCenterZ),
                 boundingBoxCenterElement, "Attribute " + std::string(ATTRIBUTE::z) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(boundingBoxDimensionElement, ATTRIBUTE::width, modelParameters.width),
                 boundingBoxCenterElement, "Attribute " + std::string(ATTRIBUTE::width) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(boundingBoxDimensionElement, ATTRIBUTE::length, modelParameters.length),
                 boundingBoxCenterElement, "Attribute " + std::string(ATTRIBUTE::length) + " is missing.");
    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(boundingBoxDimensionElement, ATTRIBUTE::height, modelParameters.height),
                 boundingBoxCenterElement, "Attribute " + std::string(ATTRIBUTE::height) + " is missing.");

    if (bbCenterY != 0.0)
    {
        LOG_INTERN(LogLevel::Warning) << "Model bounding box center y-coordinate != 0.0";
    }

    if (std::abs(bbCenterZ - modelParameters.height / 2.0) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) << "Model bounding box center z-coordinate is not half height";
    }

    modelParameters.distanceReferencePointToLeadingEdge = bbCenterX + modelParameters.length / 2.0;
}

void VehicleModelsImporter::ImportVehicleModelPerformance(QDomElement& vehicleElement,
        VehicleModelParameters& modelParameters)
{
    QDomElement performanceElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(vehicleElement, TAG::performance, performanceElement),
                 vehicleElement, "Tag " + std::string(TAG::performance) + " is missing.");

    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(performanceElement, ATTRIBUTE::maxSpeed, modelParameters.maxVelocity),
                 performanceElement, "Attribute " + std::string(ATTRIBUTE::maxSpeed) + " is missing.");

    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(performanceElement, ATTRIBUTE::mass, modelParameters.weight),
                 performanceElement, "Attribute " + std::string(ATTRIBUTE::mass) + " is missing.");
}

void VehicleModelsImporter::ImportVehicleModelGears(QDomElement& parametersElement,
        VehicleModelParameters& modelParameters)
{
    double gearRatio;

    modelParameters.gearRatios.push_back(0.0);

    ImportModelParameter(parametersElement, "NumberOfGears", modelParameters.numberOfGears);

    for (int currentGear = 1; currentGear <= modelParameters.numberOfGears; ++currentGear)
    {
        ImportModelParameter(parametersElement, "GearRatio" + std::to_string(currentGear), gearRatio);
        modelParameters.gearRatios.push_back(gearRatio);
    }
}

void VehicleModelsImporter::ImportVehicleModel(QDomElement& vehicleElement, VehicleModelMap& vehicleModelsMap)
{
    VehicleModelParameters modelParameters;

    std::string vehicleModelCategory;

    if (!SimulationCommon::ParseAttributeString(vehicleElement, ATTRIBUTE::category, vehicleModelCategory))
    {
        LOG_INTERN(LogLevel::Warning) << "Unable to parse vehicle model category";
        return;
    }

    modelParameters.vehicleType = vehicleTypeConversionMap.at(vehicleModelCategory);

    std::string vehicleModelName;

    ThrowIfFalse(SimulationCommon::ParseAttributeString(vehicleElement, ATTRIBUTE::name, vehicleModelName),
                 vehicleElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");
    ThrowIfFalse(vehicleModelsMap.find(vehicleModelName) == vehicleModelsMap.end(),
                 vehicleElement, "Vehicle model '" + vehicleModelName + "' already exists");

    QDomElement parametersElement;
    if (!SimulationCommon::GetFirstChildElement(vehicleElement, TAG::parameterDeclaration, parametersElement))
    {
        return;
    }

    ThrowIfFalse((modelParameters.vehicleType == AgentVehicleType::Car ||
                  modelParameters.vehicleType == AgentVehicleType::Truck ||
                  modelParameters.vehicleType == AgentVehicleType::Motorbike ||
                  modelParameters.vehicleType == AgentVehicleType::Bicycle),
                  vehicleElement, "VehicleModelCatagory '" + vehicleModelCategory + "' currently not supported");

    ImportModelParameter(parametersElement, "SteeringRatio", modelParameters.steeringRatio, 1.0);

    ImportModelParameter(parametersElement, "MomentInertiaRoll", modelParameters.momentInertiaRoll, 0.0);
    ImportModelParameter(parametersElement, "MomentInertiaPitch", modelParameters.momentInertiaPitch, 0.0);
    ImportModelParameter(parametersElement, "MomentInertiaYaw", modelParameters.momentInertiaYaw, 0.0);

    ImportModelParameter(parametersElement, "MinimumEngineSpeed", modelParameters.minimumEngineSpeed, 0.0);
    ImportModelParameter(parametersElement, "MaximumEngineSpeed", modelParameters.maximumEngineSpeed, 0.0);
    ImportModelParameter(parametersElement, "MaximumEngineTorque", modelParameters.maximumEngineTorque, 0.0);
    ImportModelParameter(parametersElement, "MinimumEngineTorque", modelParameters.minimumEngineTorque, 0.0);

    ImportModelParameter(parametersElement, "AirDragCoefficient", modelParameters.airDragCoefficient, 0.0);
    ImportModelParameter(parametersElement, "AxleRatio", modelParameters.axleRatio, 1.0);
    ImportModelParameter(parametersElement, "DecelerationFromPowertrainDrag", modelParameters.decelerationFromPowertrainDrag, 0.0);
    ImportModelParameter(parametersElement, "FrictionCoefficient", modelParameters.frictionCoeff, 1.0);
    ImportModelParameter(parametersElement, "FrontSurface", modelParameters.frontSurface, 0.0);

    ImportModelBoundingBox(vehicleElement, modelParameters);
    ImportVehicleModelAxles(vehicleElement, modelParameters);
    ImportVehicleModelPerformance(vehicleElement, modelParameters);
    ImportVehicleModelGears(parametersElement, modelParameters);

    modelParameters.heightCOG = 0.0;    // currently not supported

    CheckModelParameters(modelParameters);

    vehicleModelsMap[vehicleModelName] = modelParameters;
}

void VehicleModelsImporter::ImportPedestrianModel(QDomElement& pedestrianElement, VehicleModelMap& vehicleModelsMap)
{
    VehicleModelParameters modelParameters;

    std::string pedestrianCategory;

    ThrowIfFalse(SimulationCommon::ParseAttributeString(pedestrianElement, ATTRIBUTE::category, pedestrianCategory),
                 pedestrianElement, "Attribute " + std::string(ATTRIBUTE::category) + " is missing.");
    ThrowIfFalse(pedestrianCategory == "pedestrian", pedestrianElement, "Unsupported pedestrian catagory");

    std::string pedestrianModelName;
    ThrowIfFalse(SimulationCommon::ParseAttributeString(pedestrianElement, ATTRIBUTE::name, pedestrianModelName),
                 pedestrianElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

    ThrowIfFalse(vehicleModelsMap.find(pedestrianModelName) == vehicleModelsMap.end(),
                 pedestrianElement, "pedestrian model '" + pedestrianModelName + "' already exists");

    ThrowIfFalse(SimulationCommon::ParseAttributeDouble(pedestrianElement, ATTRIBUTE::mass, modelParameters.weight),
                 pedestrianElement, "Attribute " + std::string(ATTRIBUTE::mass) + " is missing.");

    QDomElement parametersElement;
    if (!SimulationCommon::GetFirstChildElement(pedestrianElement, TAG::parameterDeclaration, parametersElement))
    {
        return;
    }

    ImportModelBoundingBox(pedestrianElement, modelParameters);

    // Currently, AgentAdapter and Dynamics cannot handle pedestrians properly
    // setting some required defaults here for now for compatibility with cars
    modelParameters.vehicleType = AgentVehicleType::Car;
    modelParameters.gearRatios.push_back(0.0);
    modelParameters.gearRatios.push_back(1.0);
    modelParameters.numberOfGears = 1;
    modelParameters.maximumEngineTorque = 100;
    modelParameters.maximumEngineSpeed = 1e5;
    modelParameters.minimumEngineSpeed = 1;

    vehicleModelsMap[pedestrianModelName] = modelParameters;
}

} //namespace Importer
