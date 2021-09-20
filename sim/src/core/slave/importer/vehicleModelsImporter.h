/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <assert.h>
#include <unordered_map>

#include <QDomDocument>
#include <QFile>

#include "common/globalDefinitions.h"
#include "vehicleModels.h"
#include "importer/importerCommon.h"
#include "oscImporterCommon.h"

namespace Importer {

const std::unordered_map<std::string, AgentVehicleType> vehicleTypeConversionMap = {{"car", AgentVehicleType::Car},
                                                                                    {"van", AgentVehicleType::Car},
                                                                                    {"truck", AgentVehicleType::Truck},
                                                                                    {"bus", AgentVehicleType::Truck},
                                                                                    {"motorbike", AgentVehicleType::Motorbike},
                                                                                    {"bicycle", AgentVehicleType::Bicycle}};

using Properties = std::map<std::string, openScenario::ParameterizedAttribute<double>>;

/*!
 * \brief Provides methods for importing vehicle models from OpenSCENARIO catalog files
 */
class VehicleModelsImporter
{
public:
    /*!
     * \brief Imports vehicle and pedestrian models from the given paths
     *
     * The `vehicleModelsPath` and the `pedestrianModelsPath` are appended to the configurationDir string, respectively.
     * If one of the paths is an empty string, the import of this type of models is ommited and doesn't result
     * in an error.
     *
     * \param[in]   vehicleModelsPath       File path to the vehicle model catalog
     * \param[in]   pedestrianModelsPath    File path to the pedestrian model catalog
     * \param[out]  vehicleModels           Storage for the imported models
     *
     * \return      true on success, false otherwise
     */
    static bool Import(const std::string& vehicleModelsPath,
                       const std::string& pedestrianModelsPath,
                       Configuration::VehicleModels& vehicleModels);

    /*!
     * \brief Imports a vehicle model from OpenSCENARIO DOM
     *
     * The model is stored in `vehicleModelsMap` with the model attribute `name` as its key.
     * Duplicated model names are not allowed.
     *
     * Supported vehicle model categories are `car`, `van`, `truck`, `bus`, `motorbike` and `bicycle`.
     *
     * \param[in]   modelElement        Vehicle model DOM element
     * \param[out]  vehicleModelsMap    Storage for the imported vehicle model
     *
     * \throw   std::runtime_error  On invalid XML, duplicated model names or unsupported vehicle category
     */
    static void ImportVehicleModel(QDomElement& modelElement, VehicleModelMap& vehicleModelsMap);

    /*!
     * \brief Imports a pedestrian model from OpenSCENARIO DOM
     *
     * The model is stored in `vehicleModelsMap` with the model attribute `name` as its key.
     * Duplicated model names are not allowed.
     *
     * The only supported pedestrian model category is `pedestrian`.
     *
     * \param[in]   modelElement        Pedestrian model DOM element
     * \param[out]  vehicleModelsMap    Storage for the imported pedestrian model (NOTE: internaly shared with vehicles)
     *
     * \throw   std::runtime_error  On invalid XML, duplicated model names or unsupported pedestrian category
     */
    static void ImportPedestrianModel(QDomElement& pedestrianElement, VehicleModelMap& vehicleModelsMap);

private:
    /*!
     * \brief Asserts that the provided model parameters are valid
     *
     * Model parameters are considered valid, if all the following parameters have a value greater than 0.0
     * and the vehicle type is not `AgentVehicleType::Undefined`:
     *     - length
     *     - width
     *     - height
     *     - trackwidth
     *     - wheelbase
     *     - maxVelocity
     *     - maxCurvature
     *
     * \param[in]   model   The model parameters to check
     */
    static void CheckModelParameters(const ParametrizedVehicleModelParameters& model);

    /*!
     * \brief Imports a catalog from OpenSCENARIO DOM
     *
     *
     * \param[in]   catalogPath         Full path to the catalog XML file
     * \param[out]  catalogElement      The imported catalog element
     *
     * \throw   std::runtime_error  On invalid XML or missing catalog tag
     */
    static void ImportCatalog(const std::string& catalogPath, QDomElement& catalogElement);

    /*!
     * \brief Imports all vehicles from a catalog (OpenSCENARIO DOM)
     *
     * \param[in]   catalogElement      The catalog element to import vehicles from
     * \param[out]  vehicleModels       Storage for the imported vehicle models
     */
    static void ImportVehicleCatalog(QDomElement& catalogElement, Configuration::VehicleModels& vehicleModels);

    /*!
     * \brief Imports all pedestrians from a catalog (OpenSCENARIO DOM)
     *
     * \param[in]   catalogElement      The catalog element to import predestrians from
     * \param[out]  vehicleModels       Storage for the imported pedestrian models (NOTE: internaly shared with vehicles)
     */
    static void ImportPedestrianCatalog(QDomElement& catalogElement, Configuration::VehicleModels& vehicleModels);

    /*!
     * \brief Imports the model's bounding box from OpenSCENARIO DOM
     *
     * \param[in]   modelElement        The DOM element of the model
     * \param[out]  modelParameters     Storage for the imported values
     * \param[in]   parameters          declared parameters
     *
     * A warning is issued, if the bounding box center is not 0.0 in y-direction and half the model's height in z-direction,
     * as offsets in these directions are not supported by the simulator.
     *
     * \throw   std::runtime_error  On missing `BoundingBox`, `Center` or `Dimension` tag
     */
    static void ImportModelBoundingBox(QDomElement& modelElement, ParametrizedVehicleModelParameters& modelParameters, openScenario::Parameters& parameters);

    /*!
     * \brief Imports the models' axles parameters
     *
     * The following parameters are derived from axle configuration:
     *     - maxCurvature
     *     - maximumSteeringWheelAngleAmplitude
     *     - trackWidth
     *     - wheelBase
     *     - distanceReferencePointToFrontAxle
     *     - staticWheelRadius
     *
     * Warnings are issued for unsupported configurations (different track width for axles, different radius for front and
     * rear wheels or front axle located behind rear axle).
     *
     * \param[in]   vehicleElement      The DOM element of the vehicle model
     * \param[out]  modelParameters     Storage for the imported values
     * \param[in]   parameters          declared parameters
     *
     * \throw   std::runtime_error  On missing `Axles`, `Front` or `Rear` tag, reference point not being on rear axle or
     *                              steering defined for rear axle
     */
    static void ImportVehicleModelAxles(QDomElement& vehicleElement, ParametrizedVehicleModelParameters& modelParameters, openScenario::Parameters& parameters);


    /*!
     * \brief Imports the parameters of a single axle
     *
     * \param[in]   axleElement         The DOM element ot the axle
     * \param[out]  axleParameters      Storage for the imported values
     * \param[in]   parameters          declared parameters
     *
     * \throw   std::runtime_error  On missing `Axles`, `Front` or `Rear` tag, reference point not being on rear axle or
     *                              steering defined for rear axle
     */
    static void ImportVehicleModelAxle(QDomElement& axleElement, ParametrizedVehicleModelParameters::Axle& axleParameters, openScenario::Parameters& parameters);

    /*!
     * \brief Validates the axle parameters
     *
     * \param[in]   frontAxle   The parameters of the front axle
     * \param[in]   rearAxle    The parameters of the rear axle
     *
     * \throw std::runtime_error    When reference point not on rear axle or rear axle steering != 0.0
     */
    static void ValidateAxles(const ParametrizedVehicleModelParameters::Axle& frontAxle, const ParametrizedVehicleModelParameters::Axle& rearAxle);

    /*!
     * \brief Imports the models' performance tag from OpenSCENARIO DOM
     *
     * The values for `maxSpeed` and `mass` are imported to their corresponding model parameters.
     *
     * \note  Currently, `maxDeceleration` is ignored.
     *
     * \param[in]   vehicleElement      The DOM element of the vehicle model
     * \param[out]  modelParameters     Storage for the imported values
     * \param[in]   parameters          declared parameters
     *
     * \throw   std::runtime_error  On missing `Performance` tag or missing or invalid `maxSpeed` or `mass` attribute
     */
    static void ImportVehicleModelPerformance(QDomElement& vehicleElement, ParametrizedVehicleModelParameters& modelParameters, openScenario::Parameters& parameters);

    /*!
     * \brief Helper for template type deduction with std::optional parameters
     *
     * As template type deduction is only able to directly match types, the type passed std::optional arguments has to be
     * stated explicitly. This wrapper allows to circumvent this restriction.
     *
     * \see ImportModelParameter
     */
    template<typename T>
    struct TypeHelper { typedef T type; };

    /*!
     * \brief Imports the properties of an element
     *
     * \param[in]   root       XML document root
     *
     * \return   Properties
     */
    static Properties ImportProperties(QDomElement& root);
};

} //namespace Importer
