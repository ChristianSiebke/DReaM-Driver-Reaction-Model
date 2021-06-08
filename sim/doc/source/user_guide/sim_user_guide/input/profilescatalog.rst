..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _profilescatalog:

ProfilesCatalog
===============

The ProfilesCatalog contains all AgentProfiles, VehicleProfiles and generic ProfileGroups and Profiles. 
Depending on the configuration the simulator could require a "Driver"-ProfileGroup, a "Spawner"- and "TrafficGroup"-ProfileGroup, or sensor and vehiclecomponent specific ProfileGroups.

* :ref:`profilescatalog_agentprofiles`
* :ref:`profilescatalog_vehicleprofiles`
* :ref:`profilescatalog_profilegroups`
* :ref:`profilescatalog_driverprofiles`
* :ref:`profilescatalog_vehiclecomponentprofiles`
* :ref:`profilescatalog_spawnerprofiles`

.. _profilescatalog_agentprofiles:

AgentProfiles
-------------

In this section all AgentProfiles are defined. 
An AgentProfile is either static or dynamic. 
A static AgentProfile consists of a SystemConfig and a VehicleModel.
A dynamic AgentProfile specifies the composition of the agent according to certain probabilities.
Here the initial driver and the vehicle profile of an agent get specified.
At least one AgentProfile is required. 
But every AgentProfile referenced in the used :ref:`scenario` file or :ref:`components_trafficgroups` must exist.
All probabilities must add up to 1.0.

Composition of a static AgentProfile:

+-----------------+-----------------------------------------------------------------------+-----------+
| Tag             | Description                                                           | Required  |
+=================+=======================================================================+===========+
| System          | Reference to a system defined in a SystemConfig                       | Yes       |
+-----------------+-----------------------------------------------------------------------+-----------+
| VehicleModel    | Name of the VehicleModel in the :ref:`scenario_vehiclemodels`         | Yes       |
+-----------------+-----------------------------------------------------------------------+-----------+

Composition of a dynamic AgentProfile:

+-----------------+---------------------------------------------------------------------------------+--------------------+
| Tag             | Description                                                                     | Required           |
+=================+=================================================================================+====================+
| DriverProfiles  | List of :ref:`profilescatalog_driverprofiles` for random selection              | At least one entry |
+-----------------+---------------------------------------------------------------------------------+--------------------+
| VehicleProfiles | List of :ref:`profilescatalog_vehicleprofiles` for random selection             | At least one entry |
+-----------------+---------------------------------------------------------------------------------+--------------------+

Example
~~~~~~~

In this experiment the ego agent is defined by the system with Id 0 in systemConfig.xml and the VehicleModel car_bmw_7.
Every LuxuryClassCarAgent has the driver profile "AgentFollowingDriver".
Regarding the vehicle profile 50% have a MINI Cooper and the other 50% drive a BMW 7.

.. code-block:: xml

   <AgentProfiles>
       <AgentProfile Name="EgoAgent" Type="Static">
           <System>
               <File>systemConfig.xml</File>
               <Id>0</Id>
           </System>
           <VehicleModel>car_bmw_7</VehicleModel>
       </AgentProfile>
       <AgentProfile Name="LuxuryClassCarAgent" Type="Dynamic">
           <DriverProfiles>
               <DriverProfile Name="AgentFollowingDriver" Probability="1.0"/>
           </DriverProfiles>
           <VehicleProfiles>
               <VehicleProfile Name="MINI Cooper" Probability="0.5"/>
               <VehicleProfile Name="BMW 7" Probability="0.5"/>
           </VehicleProfiles>
       </AgentProfile>
   </AgentProfiles>

.. _profilescatalog_vehicleprofiles:

VehicleProfiles
---------------

This sections contains all vehicle profiles. 
Every VehicleProfile used by :ref:`profilescatalog_agentprofiles` must be listed here.

.. code-block:: xml

   <VehicleProfiles>
      <VehicleProfile Name="BMW 7">
          <Model Name="car_bmw_7"/>
          <Components>
                 <Component Type="AEB">
                  <Profiles>
                      <Profile Name="AebProfile" Probability="0.5"/>
                  </Profiles>
                  <SensorLinks>
                      <SensorLink SensorId="0" InputId="Camera"/>
                  </SensorLinks>
                 </Component>
          </Components>
          <Sensors>
              <Sensor Id="0">
                  <Position Name="Default" Longitudinal="0.0" Lateral="0.0" Height="0.5" Pitch="0.0" Yaw="0.0" Roll="0.0"/>
                  <Profile Type="Geometric2D" Name="Standard"/>
              </Sensor>
          </Sensors>
      </VehicleProfile>
      ...
   </VehicleProfiles>

+-------------+--------------------------------------------------------+
| Attribute   | Description                                            |
+=============+========================================================+
| Name        | Name of the vehicle profile                            |
+-------------+--------------------------------------------------------+
| Components  | Lists all ADAS and other components in the vehicle     |
+-------------+--------------------------------------------------------+
| Sensors     | Lists all sensors in the vehicle                       |
+-------------+--------------------------------------------------------+

.. _profilescatalog_components:

Components
~~~~~~~~~~

+-------------+-------------------------------------------------------------------------------------------------+
| Attribute   | Description                                                                                     |
+=============+=================================================================================================+
| Type        | Type of the component.                                                                          |
|             |                                                                                                 |
|             | Must match component name in SystemConfigBlueprint                                              |
+-------------+-------------------------------------------------------------------------------------------------+
| Profile     | Possible profiles of the component with probabilities.                                          |
|             |                                                                                                 |
|             | The profiles are defined in the :ref:`profilescatalog_vehiclecomponentprofiles` section         |
|             |                                                                                                 |
|             | Probabilities do not need to add up to 1.                                                       |
+-------------+-------------------------------------------------------------------------------------------------+
| SensorLinks | Defines which sensor this component uses as input                                               |
+-------------+-------------------------------------------------------------------------------------------------+

.. note:: The possibility of "not having that component" can be achieved if the probabilities of the profiles do not add up to 1.

.. _profilescatalog_sensors:

Sensors
~~~~~~~

+-------------+-------------------------------------------------------------------------------------------------+
| Attribute   | Description                                                                                     |
+=============+=================================================================================================+
| Id          | Identifier for the sensor used by the SensorLink definition of the components                   |
+-------------+-------------------------------------------------------------------------------------------------+
| Position    | Position of the sensor in the vehicle in relative coordinates                                   |
+-------------+-------------------------------------------------------------------------------------------------+
| Profile     | All possible profiles of the sensor with probabilities.                                         |
|             |                                                                                                 |
|             | The profiles are defined in the :ref:`profilescatalog_vehiclecomponentprofiles` section         |
+-------------+-------------------------------------------------------------------------------------------------+

.. _profilescatalog_profilegroups:

ProfileGroups
-------------

A ProfileGroup defines all the possible profiles of a component.
A single profile is a set of parameters that are passed to the component in the same way as the parameters in the SystemConfig.
Note: For components that have their parameters defined in the ProfilesCatalog the parameters in the SystemConfigBlueprint are ignored.
Parameters can either be simple or stochastic.
Simple parameters only have one value, while stochastic parameters have a minimum and maximum value as well as distribution specific parameters.
Which parameters are needed/supported depends on the component.

.. code-block:: xml

   <ProfileGroup Type="ComponentName">
       <Profile Name="ExampleProfile">
           <String Key="StringParameter" Value="Lorem ipsum"/>
           <DoubleVector Key="DoubleParameter" Value="12.3,4.56,78.9"/>
           <NormalDistribution Key="RandomParameter" Mean="4.5" SD="0.5" Min="3.5" Max="10.0"/>
       </Profile>
       <Profile Name="AnotherProfile">
           ...
       </Profile>
   </ProfileGroup>

There are the following types of simple parameters:

* Bool
* Int
* Double
* String
* IntVector
* DoubleVector
* StringVector

If a parameter is stochastic it can be defined as any to be drawn from any of the following distributions:

+-------------------------+--------------------------------------------------+
| Distribution            | Additional Attributes                            |
+=========================+==================================================+
| NormalDistribution      | (Mean and SD) or (Mu and Sigma) - equivalent     |
+-------------------------+--------------------------------------------------+
| LogNormalDistribution   | (Mean and SD) or (Mu and Sigma) - not equivalent |
+-------------------------+--------------------------------------------------+
| UniformDistribution     | None                                             |
+-------------------------+--------------------------------------------------+
| ExponentialDistribution | Lambda or Mean (Mean = 1 / Lambda)               |
+-------------------------+--------------------------------------------------+
| GammaDistribution       | (Mean and SD) or (Shape and Scale)               |
+-------------------------+--------------------------------------------------+

Additionally there is the list type.
The list contains any number of list items which itself contain a list of parameters.
Lists can be nested at most two times.

.. code-block:: xml

   <List Name="ExampleList">
       <ListItem>
           <String Key="FirstParameter" Value="Lorem"/>
           <Double Key="SecondParameter" Value="0.4"/>
       </ListItem>
       <ListItem>
           <String Key="FirstParameter" Value="ipsum"/>
           <Double Key="SecondParameter" Value="0.6"/>
       </ListItem>
   </List>


A Profile can also reference another Profile in another ProfileGroup.
In these case the importer handles the reference as if it was substituted by all subelements of the referenced Profile.
References may not be nested.

.. code-block:: xml

   <Reference Type="GroupType" Name="ProfileName"/>


.. _profilescatalog_driverprofiles:

Driver ProfileGroup
-------------------

This section contains all driver profiles used by the simulation. 
At least one driver profile is required.
The special parameter "Type" defines the name of the component (i.e. library name).
For details on the indivual parameters see the :ref:`components reference <components_driver>`.

.. code-block:: xml

  <ProfileGroup Type="Driver">
    <Profile Name="Name">
      <String Key="Type" Value="DriverLibrary"/>
      ...
    </Profile>
    ...
  </ProfileGroup>


.. _profilescatalog_vehiclecomponentprofiles:

VehicleComponent ProfileGroups
------------------------------

This sections contains all driver assistance systems and other vehicle components and their parameter sets.
For every used VehicleComponent type there must be a ProfileGroup with this type and every profile of this type used by :ref:`profilescatalog_vehicleprofiles` must be listed here.
For details on the indivual parameters see the :ref:`components reference <components_vehiclecomponents>`.

.. code-block:: xml

  <ProfileGroup Type="LibraryName">
    <Profile Name="Name">
      ...
    </Profile>
    ...
  </ProfileGroup>


.. _profilescatalog_spawnerprofiles:

SpawnerProfile ProfileGroup
---------------------------

This sections contains all parameters of the spawners referenced in the :ref:`slaveconfig`.
For details on the indivual parameters see the :ref:`components reference <components_spawner>`.

.. code-block:: xml

  <ProfileGroup Type="Spawner">
    <Profile Name="Name">
      ...
    </Profile>
    ...
  </ProfileGroup>
