/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "profiles.h"
#include "parameters.h"
#include "parameterImporter.h"
#include "xmlParser.h"

//using namespace Configuration;

namespace Importer {

class ProfilesImporter
{
public:
    /*!
    * \brief Imports the AgentProfiles section of the profiles catalog
    *
    * @param[in]     agentProfilesElement    Element containing the information
    * @param[out]    agentProfiles           Map into which the values get saved
    * @return	     true, if successful
    */
    static void ImportAgentProfiles(QDomElement agentProfilesElement,
                                    std::unordered_map<std::string, AgentProfile> &agentProfiles);

    /*!
     * \brief ImportSpawnPointProfiles Imports the SpawnPointProfiles section of the profiles catalog
     * \param spawnPointProfilesElement Element containing the information
     * \param spawnPointProfiles Map into which the values get saved
     */
    static void ImportSpawnPointProfiles(const QDomElement& spawnPointProfilesElement, SpawnPointProfiles& spawnPointProfiles);

    /*!
    * \brief Imports the DriverProfiles section of the profiles catalog
    *
    * @param[in]     driverProfilesElement    Element containing the information
    * @param[out]    driverProfiles           Map into which the values get saved
    * @return	     true, if successful
    */
    static void ImportDriverProfiles(QDomElement driverProfilesElement,
                                                           DriverProfiles &driverProfiles);


    /*!
    * \brief Imports the VehicleComponentProfiles section of the profiles catalog
    *
    * @param[in]     vehicleComponentProfilesElement    Element containing the information
    * @param[out]    vehicleComponentProfilesMap        Map into which the values get saved
    * @return	     true, if successful
    */
    static void ImportAllVehicleComponentProfiles(QDomElement vehicleComponentProfilesElement,
                                                  std::unordered_map<std::string, VehicleComponentProfiles> &vehicleComponentProfilesMap);

    /*!
    * \brief Imports the SensorProfiles section of the profiles catalog
    *
    * @param[in]     sensorProfilesElement    Element containing the information
    * @param[out]    sensorProfiles           Map into which the values get saved
    * @return	     true, if successful
    */
    static void ImportSensorProfiles(QDomElement sensorProfilesElement,
                                     openpass::sensors::Profiles &sensorProfiles);

    /*!
    * \brief Imports the AgentProfiles section of the profiles catalog
    *
    * @param[in]     agentProfilesElement    Element containing the information
    * @param[out]    agentProfiles           Map into which the values get saved
    * @return	     true, if successful
    */
    static void ImportVehicleProfiles(QDomElement vehicleProfilesElement,
                                         std::unordered_map<std::string, VehicleProfile> &vehicleProfiles);

    /*!
     * \brief Imports a single VehicleProfile
     * \param vehicleProfileElement  Element containing the information
     * \param vehicleProfile         VehicleProfile to fill
     * \return
     */
    static VehicleProfile ImportVehicleProfile(QDomElement vehicleProfileElement);

    /*!
     * \brief Imports all VehicleComponentes contained in one VehicleProfile
     * \param vehicleProfileElement  Element containing the information
     * \param vehicleProfile         VehicleProfile to fill
     * \return
     */
    static void ImportAllVehicleComponentsOfVehicleProfile(QDomElement vehicleProfileElement,
                                                           VehicleProfile &vehicleProfile);

    /*!
     * \brief Imports a single VehicleComponentes contained in one VehicleProfile
     * \param vehicleComponentElement  Element containing the information
     * \param vehicleComponent         VehicleComponent to fill
     * \return
     */
    static void ImportVehicleComponent(QDomElement vehicleComponentElement,
                                       VehicleComponent &vehicleComponent);


    /*!
     * \brief Imports all SensorLinks of a VehicleComponents contained in one VehicleProfile
     * \param sensorLinksElement      Element containing the information
     * \param sensorLinksElement      Map into which SensorLinks are saved
     * \return
     */
    static void ImportSensorLinksOfComponent(QDomElement sensorLinksElement,
                                             std::list<SensorLink> &sensorLinks);

    /*!
     * \brief Imports all Sensor contained in one VehicleProfiles
     * \param vehicleProfileElement  Element containing the information
     * \param vehicleProfile         VehicleProfile to fill
     * \return
     */
    static void ImportAllSensorsOfVehicleProfile(QDomElement vehicleProfileElement,
                                                 VehicleProfile &vehicleProfile);

    /*!
     * \brief Imports a single Sensor contained in one VehicleProfile
     * \param sensorElement           Element containing the information
     * \param sensorParameter         SensorParameter to fill
     * \return
     */
    static void ImportSensorParameters(QDomElement sensorElement,
                                       openpass::sensors::Parameter &sensorParameter);

    //Overall import function
    /*!
    * \brief Imports the entire profiles catalog
    * \details Calls all sections specific methods and saves the result in the CombiantionConfig
    *
    * @param[in]     filename                Name of the SlaveConfig file
    * @param[out]    slaveConfig       Calss into which the values get saved
    * @return	     true, if successful
    */
    static bool Import(const std::string &filename, Profiles &profiles);

private:
    static constexpr auto profilesCatalogFile = "ProfilesCatalog.xml";
    static constexpr auto supportedConfigVersion = "0.3.0";

};
} //namespace Importer
