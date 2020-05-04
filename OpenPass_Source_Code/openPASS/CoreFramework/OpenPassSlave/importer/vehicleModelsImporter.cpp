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

void VehicleModelsImporter::CheckModelParameters(const ParametrizedVehicleModelParameters& model)
{
    //TODO Replace assert with ErrorMessage
    assert(model.length.defaultValue > 0);
    assert(model.width.defaultValue > 0);
    assert(model.height.defaultValue > 0);
    assert(model.rearAxle.trackWidth.defaultValue > 0);
//    assert(model.wheelbase.defaultValue > 0);
    assert(model.maxVelocity.defaultValue > 0);
//    assert(model.maxCurvature.defaultValue > 0);
    assert(model.vehicleType != AgentVehicleType::Undefined);
}

void VehicleModelsImporter::ImportVehicleModelAxles(QDomElement& vehicleElement,
                                                    ParametrizedVehicleModelParameters& modelParameters,
                                                    openScenario::Parameters& parameters)
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

    ImportVehicleModelAxle(frontAxleElement, modelParameters.frontAxle, parameters);
    ImportVehicleModelAxle(rearAxleElement, modelParameters.rearAxle, parameters);
    ValidateAxles(modelParameters.frontAxle, modelParameters.rearAxle);
}

void VehicleModelsImporter::ImportVehicleModelAxle(QDomElement& axleElement, VehicleAxle& axleParameters, openScenario::Parameters& parameters)
{
    axleParameters.wheelDiameter = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::wheelDiameter, parameters);
    axleParameters.positionX = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::positionX, parameters);
    axleParameters.trackWidth = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::trackWidth, parameters);
    axleParameters.maxSteering = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::maxSteering, parameters);
}

void VehicleModelsImporter::ValidateAxles(const VehicleAxle& frontAxle, const VehicleAxle& rearAxle)
{
    if (std::abs(frontAxle.wheelDiameter.defaultValue - rearAxle.wheelDiameter.defaultValue) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) <<
                                      "Different wheel diameters for front and rear axle not supported. Using rear axle value.";
    }

    ThrowIfFalse(rearAxle.positionX.defaultValue == 0.0, "Reference point not on rear axle.");

    if (rearAxle.positionX.defaultValue > frontAxle.positionX.defaultValue)
    {
        LOG_INTERN(LogLevel::Warning) << "Front axle is located behind rear axle.";
    }

    if (std::abs(frontAxle.trackWidth.defaultValue - rearAxle.trackWidth.defaultValue) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) << "Different track width for front and rear axle not supported. Using rear axle value.";
    }

    ThrowIfFalse(rearAxle.maxSteering.defaultValue == 0.0, "Rear axle steering not supported.");
}

void VehicleModelsImporter::ImportModelBoundingBox(QDomElement& modelElement, ParametrizedVehicleModelParameters& modelParameters, openScenario::Parameters& parameters)
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

    auto bbCenterX = ParseParametrizedAttribute<double>(boundingBoxCenterElement, ATTRIBUTE::x, parameters);
    auto bbCenterY = ParseParametrizedAttribute<double>(boundingBoxCenterElement, ATTRIBUTE::y, parameters);
    auto bbCenterZ = ParseParametrizedAttribute<double>(boundingBoxCenterElement, ATTRIBUTE::z, parameters);
    modelParameters.width = ParseParametrizedAttribute<double>(boundingBoxDimensionElement, ATTRIBUTE::width, parameters);
    modelParameters.length = ParseParametrizedAttribute<double>(boundingBoxDimensionElement, ATTRIBUTE::length, parameters);
    modelParameters.height = ParseParametrizedAttribute<double>(boundingBoxDimensionElement, ATTRIBUTE::height, parameters);

    if (bbCenterY.defaultValue != 0.0)
    {
        LOG_INTERN(LogLevel::Warning) << "Model bounding box center y-coordinate != 0.0";
    }

    if (std::abs(bbCenterZ.defaultValue - modelParameters.height.defaultValue / 2.0) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) << "Model bounding box center z-coordinate is not half height";
    }

    modelParameters.distanceReferencePointToLeadingEdge = bbCenterX.defaultValue + modelParameters.length.defaultValue / 2.0;
}

void VehicleModelsImporter::ImportVehicleModelPerformance(QDomElement& vehicleElement,
                                                          ParametrizedVehicleModelParameters& modelParameters,
                                                          openScenario::Parameters& parameters)
{
    QDomElement performanceElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(vehicleElement, TAG::performance, performanceElement),
                 vehicleElement, "Tag " + std::string(TAG::performance) + " is missing.");

    modelParameters.maxVelocity = ParseParametrizedAttribute<double>(performanceElement, ATTRIBUTE::maxSpeed, parameters);

    modelParameters.weight = ParseParametrizedAttribute<double>(performanceElement, ATTRIBUTE::mass, parameters);
}

void VehicleModelsImporter::ImportVehicleModelGears(ParametrizedVehicleModelParameters& modelParameters,
                                                    openScenario::Parameters& parameters)
{
    openScenario::ParametrizedAttribute<double> gearRatio;

    modelParameters.gearRatios.push_back(0.0);

    AssignModelParameter("NumberOfGears", modelParameters.numberOfGears, parameters);

    for (int currentGear = 1; currentGear <= modelParameters.numberOfGears.defaultValue; ++currentGear)
    {
        AssignModelParameter("GearRatio" + std::to_string(currentGear), gearRatio, parameters);
        modelParameters.gearRatios.push_back(gearRatio);
    }
}

void VehicleModelsImporter::ImportVehicleModel(QDomElement& vehicleElement, VehicleModelMap& vehicleModelsMap)
{
    ParametrizedVehicleModelParameters modelParameters;
    openScenario::Parameters parameters;

    QDomElement parametersElement;
    if (!SimulationCommon::GetFirstChildElement(vehicleElement, TAG::parameterDeclaration, parametersElement))
    {
        return;
    }

    Importer::ImportParameterDeclarationElement(parametersElement, parameters);

    auto vehicleModelCategory = ParseParametrizedAttribute<std::string>(vehicleElement, ATTRIBUTE::category, parameters);

    modelParameters.vehicleType = vehicleTypeConversionMap.at(vehicleModelCategory.defaultValue);

    auto vehicleModelName = ParseParametrizedAttribute<std::string>(vehicleElement, ATTRIBUTE::name, parameters);
    ThrowIfFalse(vehicleModelsMap.find(vehicleModelName.defaultValue) == vehicleModelsMap.end(),
                 vehicleElement, "Vehicle model '" + vehicleModelName.defaultValue + "' already exists");


    ThrowIfFalse((modelParameters.vehicleType == AgentVehicleType::Car ||
                  modelParameters.vehicleType == AgentVehicleType::Truck ||
                  modelParameters.vehicleType == AgentVehicleType::Motorbike ||
                  modelParameters.vehicleType == AgentVehicleType::Bicycle),
                  vehicleElement, "VehicleModelCatagory '" + vehicleModelCategory.defaultValue + "' currently not supported");

    AssignModelParameter("SteeringRatio", modelParameters.steeringRatio, parameters, 1.0);

    AssignModelParameter("MomentInertiaRoll", modelParameters.momentInertiaRoll, parameters, 0.0);
    AssignModelParameter("MomentInertiaPitch", modelParameters.momentInertiaPitch, parameters, 0.0);
    AssignModelParameter("MomentInertiaYaw", modelParameters.momentInertiaYaw, parameters, 0.0);

    AssignModelParameter("MinimumEngineSpeed", modelParameters.minimumEngineSpeed, parameters, 0.0);
    AssignModelParameter("MaximumEngineSpeed", modelParameters.maximumEngineSpeed, parameters, 0.0);
    AssignModelParameter("MaximumEngineTorque", modelParameters.maximumEngineTorque, parameters, 0.0);
    AssignModelParameter("MinimumEngineTorque", modelParameters.minimumEngineTorque, parameters, 0.0);

    AssignModelParameter("AirDragCoefficient", modelParameters.airDragCoefficient, parameters, 0.0);
    AssignModelParameter("AxleRatio", modelParameters.axleRatio, parameters, 1.0);
    AssignModelParameter("DecelerationFromPowertrainDrag", modelParameters.decelerationFromPowertrainDrag, parameters, 0.0);
    AssignModelParameter("FrictionCoefficient", modelParameters.frictionCoeff, parameters, 1.0);
    AssignModelParameter("FrontSurface", modelParameters.frontSurface, parameters, 0.0);

    ImportModelBoundingBox(vehicleElement, modelParameters, parameters);
    ImportVehicleModelAxles(vehicleElement, modelParameters, parameters);
    ImportVehicleModelPerformance(vehicleElement, modelParameters, parameters);
    ImportVehicleModelGears(modelParameters, parameters);

    modelParameters.heightCOG = 0.0;    // currently not supported

    CheckModelParameters(modelParameters);

    vehicleModelsMap[vehicleModelName.defaultValue] = modelParameters;
}

void VehicleModelsImporter::ImportPedestrianModel(QDomElement& pedestrianElement, VehicleModelMap& vehicleModelsMap)
{
    ParametrizedVehicleModelParameters modelParameters;
    openScenario::Parameters parameters;

    QDomElement parametersElement;
    if (!SimulationCommon::GetFirstChildElement(pedestrianElement, TAG::parameterDeclaration, parametersElement))
    {
        return;
    }

    Importer::ImportParameterDeclarationElement(parametersElement, parameters);

    auto pedestrianCategory = ParseParametrizedAttribute<std::string>(pedestrianElement, ATTRIBUTE::category, parameters);
    ThrowIfFalse(pedestrianCategory.defaultValue == "pedestrian", pedestrianElement, "Unsupported pedestrian catagory");

    auto pedestrianModelName = ParseParametrizedAttribute<std::string>(pedestrianElement, ATTRIBUTE::name, parameters);

    ThrowIfFalse(vehicleModelsMap.find(pedestrianModelName.defaultValue) == vehicleModelsMap.end(),
                 pedestrianElement, "pedestrian model '" + pedestrianModelName.defaultValue + "' already exists");

    modelParameters.weight = ParseParametrizedAttribute<double>(pedestrianElement, ATTRIBUTE::mass, parameters);

    ImportModelBoundingBox(pedestrianElement, modelParameters, parameters);

    // Currently, AgentAdapter and Dynamics cannot handle pedestrians properly
    // setting some required defaults here for now for compatibility with cars
    modelParameters.vehicleType = AgentVehicleType::Car;
    modelParameters.gearRatios.push_back(0.0);
    modelParameters.gearRatios.push_back(1.0);
    modelParameters.numberOfGears = 1;
    modelParameters.maximumEngineTorque = 100;
    modelParameters.maximumEngineSpeed = 1e5;
    modelParameters.minimumEngineSpeed = 1;

    vehicleModelsMap[pedestrianModelName.defaultValue] = modelParameters;
}

} //namespace Importer
