/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/


#include "common/opMath.h"
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
    ThrowIfFalse(model.boundingBoxDimensions.length.defaultValue > 0, "Length must be positive");
    ThrowIfFalse(model.boundingBoxDimensions.width.defaultValue > 0, "Width must be positive");
    ThrowIfFalse(model.boundingBoxDimensions.height.defaultValue > 0, "Height must be positive");
    ThrowIfFalse(model.performance.maxSpeed.defaultValue > 0, "MaxSpeed must be positive");
    ThrowIfFalse(model.vehicleType != AgentVehicleType::Undefined, "Unknown vehicle type");
}

void VehicleModelsImporter::ImportVehicleModelAxles(QDomElement& vehicleElement,
                                                    ParametrizedVehicleModelParameters& modelParameters,
                                                    openScenario::Parameters& parameters)
{
    QDomElement axlesElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(vehicleElement, TAG::axles, axlesElement),
                 vehicleElement, "Tag " + std::string(TAG::axles) + " is missing.");

    QDomElement frontAxleElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(axlesElement, TAG::frontAxle, frontAxleElement),
                 axlesElement, "Tag " + std::string(TAG::frontAxle) + " is missing.");

    QDomElement rearAxleElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(axlesElement, TAG::rearAxle, rearAxleElement),
                 axlesElement, "Tag " + std::string(TAG::rearAxle) + " is missing.");

    ImportVehicleModelAxle(frontAxleElement, modelParameters.frontAxle, parameters);
    ImportVehicleModelAxle(rearAxleElement, modelParameters.rearAxle, parameters);
    ValidateAxles(modelParameters.frontAxle, modelParameters.rearAxle);
}

void VehicleModelsImporter::ImportVehicleModelAxle(QDomElement& axleElement, ParametrizedVehicleModelParameters::Axle& axleParameters, openScenario::Parameters& parameters)
{
    axleParameters.wheelDiameter = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::wheelDiameter, parameters);
    axleParameters.positionX = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::positionX, parameters);
    axleParameters.positionZ = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::positionZ, parameters);
    axleParameters.trackWidth = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::trackWidth, parameters);
    axleParameters.maxSteering = ParseParametrizedAttribute<double>(axleElement, ATTRIBUTE::maxSteering, parameters);
}

void VehicleModelsImporter::ValidateAxles(const ParametrizedVehicleModelParameters::Axle& frontAxle, const ParametrizedVehicleModelParameters::Axle& rearAxle)
{
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
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(boundingBoxElement, TAG::dimensions, boundingBoxDimensionElement),
                 boundingBoxElement, "Tag " + std::string(TAG::dimensions) + " is missing.");

    modelParameters.boundingBoxCenter.x = ParseParametrizedAttribute<double>(boundingBoxCenterElement, ATTRIBUTE::x, parameters);
    modelParameters.boundingBoxCenter.y = ParseParametrizedAttribute<double>(boundingBoxCenterElement, ATTRIBUTE::y, parameters);
    modelParameters.boundingBoxCenter.z = ParseParametrizedAttribute<double>(boundingBoxCenterElement, ATTRIBUTE::z, parameters);
    modelParameters.boundingBoxDimensions.width = ParseParametrizedAttribute<double>(boundingBoxDimensionElement, ATTRIBUTE::width, parameters);
    modelParameters.boundingBoxDimensions.length = ParseParametrizedAttribute<double>(boundingBoxDimensionElement, ATTRIBUTE::length, parameters);
    modelParameters.boundingBoxDimensions.height = ParseParametrizedAttribute<double>(boundingBoxDimensionElement, ATTRIBUTE::height, parameters);

    if (modelParameters.boundingBoxCenter.y.defaultValue != 0.0)
    {
        LOG_INTERN(LogLevel::Warning) << "Model bounding box center y-coordinate != 0.0";
    }

    if (std::abs(modelParameters.boundingBoxCenter.z.defaultValue - modelParameters.boundingBoxDimensions.height.defaultValue / 2.0) > 1e-6)
    {
        LOG_INTERN(LogLevel::Warning) << "Model bounding box center z-coordinate is not half height";
    }
}

void VehicleModelsImporter::ImportVehicleModelPerformance(QDomElement& vehicleElement,
                                                          ParametrizedVehicleModelParameters& modelParameters,
                                                          openScenario::Parameters& parameters)
{
    QDomElement performanceElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(vehicleElement, TAG::performance, performanceElement),
                 vehicleElement, "Tag " + std::string(TAG::performance) + " is missing.");

    modelParameters.performance.maxSpeed = ParseParametrizedAttribute<double>(performanceElement, ATTRIBUTE::maxSpeed, parameters);
    modelParameters.performance.maxAcceleration = ParseParametrizedAttribute<double>(performanceElement, ATTRIBUTE::maxAcceleration, parameters);
    modelParameters.performance.maxDeceleration = ParseParametrizedAttribute<double>(performanceElement, ATTRIBUTE::maxDeceleration, parameters);
}

void VehicleModelsImporter::ImportVehicleModel(QDomElement& vehicleElement, VehicleModelMap& vehicleModelsMap)
{
    ParametrizedVehicleModelParameters modelParameters;
    openScenario::Parameters parameters;

    QDomElement parametersElement;
    if (SimulationCommon::GetFirstChildElement(vehicleElement, TAG::parameterDeclarations, parametersElement))
    {
        Importer::ImportParameterDeclarationElement(parametersElement, parameters);
    }


    auto vehicleModelCategory = ParseParametrizedAttribute<std::string>(vehicleElement, ATTRIBUTE::vehicleCategory, parameters);

    modelParameters.vehicleType = vehicleTypeConversionMap.at(vehicleModelCategory.defaultValue);

    auto vehicleModelName = ParseParametrizedAttribute<std::string>(vehicleElement, ATTRIBUTE::name, parameters);
    ThrowIfFalse(vehicleModelsMap.find(vehicleModelName.defaultValue) == vehicleModelsMap.end(),
                 vehicleElement, "Vehicle model '" + vehicleModelName.defaultValue + "' already exists");


    ThrowIfFalse((modelParameters.vehicleType == AgentVehicleType::Car ||
                  modelParameters.vehicleType == AgentVehicleType::Truck ||
                  modelParameters.vehicleType == AgentVehicleType::Motorbike ||
                  modelParameters.vehicleType == AgentVehicleType::Bicycle),
                  vehicleElement, "VehicleModelCatagory '" + vehicleModelCategory.defaultValue + "' currently not supported");

    modelParameters.properties = ImportProperties(vehicleElement);

    ImportModelBoundingBox(vehicleElement, modelParameters, parameters);
    ImportVehicleModelAxles(vehicleElement, modelParameters, parameters);
    ImportVehicleModelPerformance(vehicleElement, modelParameters, parameters);

    CheckModelParameters(modelParameters);

    vehicleModelsMap[vehicleModelName.defaultValue] = modelParameters;
}

void VehicleModelsImporter::ImportPedestrianModel(QDomElement& pedestrianElement, VehicleModelMap& vehicleModelsMap)
{
    ParametrizedVehicleModelParameters modelParameters;

    openScenario::Parameters parameters{};

    auto pedestrianCategory = ParseParametrizedAttribute<std::string>(pedestrianElement, ATTRIBUTE::pedestrianCategory, parameters);
    ThrowIfFalse(pedestrianCategory.defaultValue == "pedestrian", pedestrianElement, "Unsupported pedestrian catagory");

    auto pedestrianModelName = ParseParametrizedAttribute<std::string>(pedestrianElement, ATTRIBUTE::name, parameters);

    ThrowIfFalse(vehicleModelsMap.find(pedestrianModelName.defaultValue) == vehicleModelsMap.end(),
                 pedestrianElement, "pedestrian model '" + pedestrianModelName.defaultValue + "' already exists");

    modelParameters.properties = ImportProperties(pedestrianElement);
    ImportModelBoundingBox(pedestrianElement, modelParameters, parameters);

    // Currently, AgentAdapter and Dynamics cannot handle pedestrians properly
    // setting some required defaults here for now for compatibility with cars
    modelParameters.vehicleType = AgentVehicleType::Car;
    modelParameters.rearAxle.wheelDiameter = 1;
    modelParameters.rearAxle.positionX = 0;
    modelParameters.frontAxle.positionX = 1;

    vehicleModelsMap[pedestrianModelName.defaultValue] = modelParameters;
}

Properties VehicleModelsImporter::ImportProperties(QDomElement& root)
{
    Properties properties{};

    QDomElement propertiesElement;
    if (SimulationCommon::GetFirstChildElement(root, TAG::properties, propertiesElement))
    {
        QDomElement propertyElement;
        SimulationCommon::GetFirstChildElement(propertiesElement, TAG::property, propertyElement);

        while (!propertyElement.isNull())
        {
            std::string name;
            ThrowIfFalse(SimulationCommon::ParseAttribute(propertyElement, ATTRIBUTE::name, name),
                         propertyElement, "Attribute " + std::string(ATTRIBUTE::name) + " is missing.");

            std::string value;
            ThrowIfFalse(SimulationCommon::ParseAttribute(propertyElement, ATTRIBUTE::value, value),
                         propertyElement, "Attribute " + std::string(ATTRIBUTE::value) + " is missing.");

            properties[name] = std::stod(value);

            propertyElement = propertyElement.nextSiblingElement(TAG::property);
        }
    }

    return properties;
}

} //namespace Importer
