\page io Input & Output

\tableofcontents

\section io_input Input

In this section all input files and their contents are described. All input files need to be placed in the "/configs" subfolder.
Several tags of some input files are optional. But all attributes related to a tag are required, if this tag is used.

Obligatory input files are:
systemConfigBlueprint.xml, slaveConfig.xml, ProfilesCatalog.xml, Scenario.xosc, SceneryConfigruation.xodr, VehicleModelsCatalog.xosc and PedestrianModelsCatalog.xosc.

Optional input files are:
SystemConfig files and Trajectory files (only required if specified in the slaveConfig.xml).

For demonstration purposes all files of an AEB scenario have been added as an example.

[//]: <> (Please refer to each section!)
* [systemConfigBlueprint.xml](\ref io_input_systemconfigblueprint)
* [slaveConfig.xml](\ref io_input_slaveconfig)
* [ProfilesCatalog.xml](\ref io_input_profilescatalog)
* [Scenario.xosc](\ref io_input_scenario)
* [SceneryConfiguration.xodr](\ref io_input_scenery)
* [VehicleModelsCatalog.xosc](\ref io_input_vehiclemodels)
* [PedestrianModelsCatalog.xosc](\ref io_input_pedestrianmodels)
* [System Configuration](\ref io_input_systemconfig)

\subsection io_input_systemconfigblueprint systemConfigBlueprint.xml

This file contains the possible agent modules and channels of a dynamically generated agent. The content of this file should only be adjusted by experienced users with knowledge of the simulation architecture. The SystemConfigBlueprint is a special SystemConfig and has the same schema. Only the system with id 0 is used for generating dynamic agents.
If the simulation uses only statically configured agents (AgentProfile Type attribute is "Static"), this file isn't required.

_AgentComponents_
All components are listed here. An agent consists of a subset of this components.

|Attribute|Description|
|---------|-----------|
|Id|Used as key by the simulation to find the component|
|Priority|The module with the highest priority value gets executed first by the scheduler|
|Offset|Delay for the trigger method of each component in ms|
|Cycle|Interval in which the component gets triggered by the scheduler in ms|
|Response|Delay for the UpdateOutput method of each component in ms|
|Library|Library name of this component|
|Parameters|Parameters used by the component|

Example:
This example describes the Sensor_Driver module.

```xml
<component>
    <id>Sensor_Driver</id>
        <schedule>
            <priority>490</priority>
            <offset>0</offset>
            <cycle>100</cycle>
            <response>0</response>
        </schedule>
        <library>Sensor_Driver</library>
        <parameters/>
</component>
```

\subsubsection io_input_systemconfigblueprint_modulepriorities Priorities

Please refer to the [Components and channel communication diagram]]\ref dev_concepts_modulecomposition) for assignment of a proper priority.
Based on the signal flow, input relevant components like sensors need to be executed first. They provide data for consuming components (algorithms) like ADAS and drivers.
Data is then handled by algorithms like Algorithm_Lateral.
Output-relevant modules like dynamics and actions are executed last.

Prioritizer can be applied on different levels depending on the modules/data they need to handle levels as following can be defined:
Level 1 describes data produced by ADAS and drivers. 
Level 2 describes data output by vehicle dynamic controllers.
Level 3 describes data delivered by dynamics.

Priorities can be grouped (coarse) as following:
Highest number indicates highest priority.

| Scope                      | Range     |
|----------------------------|-----------|
| Parameters                 | 500       |
| OpenScenarioActions        | 400       |
| Sensor                     | 350...399 |
| Event                      | 330       |
| DriverModels               | 310       |
| ADAS                       | 250...299 |
| ComponentController        | 200       |
| Prioritizer (Lvl. 1)       | 150...199 |
| VehicleDynamicsControllers | 100...149 |
| Prioritizer (Lvl. 2)       | 75...99   |
| Dynamics                   | 50...74   |
| Prioritizer (Lvl. 3)       | 25...49   |
| Updater                    | 0...24    |

The table below can be used as orientation when a new module is introduced.

|Name|Library|Priority|Scope|Note|
|---|-----------|-----------|-----------|-----------|
| ParametersAgentModules| ParametersAgent| 500 | Parameters | Sets all init-data and is updated cyclically |
| OpenScenarioActions | OpenScenarioActions | 400 | ADAS | Reads events from OpenScenario Actions and forwards them to other components |
| SensorObjectDetector | Sensor_OSI | 398 | Sensor | Gets instantiated multiple time (one time per sensor) |
| SensorAggregation | SensorAggregation_OSI | 351 | Sensor | - |
| SensorFusionErrorless | SensorFusionErrorless_OSI | 350 | Sensor | - |
| AlgorithmAgentFollowingDriverModel | AlgorithmAgentFollowingDriverModel | 310 | DriverModels | - |
| AEB | AlgorithmAutonomousEmergencyBraking | 250 | ADAS | - |
| ComponentController | ComponentController | 200 | ADAS | Manages vehicle component states with regard to other vehicle component states and conditions and in response to events. |
| PrioritizerLaterDriver | SignalPrioritizer | 150 | Prioritizer | - |
| PrioritizerAccelerationDriver | SignalPrioritizer | 150 | Prioritizer | - |
| PrioritizerTurningIndicator | SignalPrioritizer | 150 | Prioritizer | - |
| PrioritizerSteeringVehicleComponents | SignalPrioritizer | 150 | Prioritizer | - |
| PrioritizerAccelerationVehicleComponents | SignalPrioritizer | 150 | Prioritizer | - |
| LimiterAccelerationVehicleComponents | LimiterAccelerationVehicleComponents | 120 | VehicleDynamicsControllers | - |
| AlgorithmLateralDriver | AlgorithmLateralDriver | 100 | VehicleDynamicsControllers | - |
| AlgorithmLongitudinalVehicleComponents | AlgorithmLongitudinalVehicleComponents | 100 | VehicleDynamicsControllers | - |
| AlgorithmLongitudinalDriver | AlgorithmLongitudinalDriver | 100 | VehicleDynamicsControllers | - |
| PrioritizerSteering | SignalPrioritizer | 75 | Prioritizer | - |
| PrioritizerLongitudinal | SignalPrioritizer | 75 | Prioritizer | - |
| DynamicsCollision | DynamicsCollision | 50 | Dynamics | - |
| DynamicsRegularDriving | DynamicsRegularDriving | 50 | Dynamics | - |
| DynamicsTrajectoryFollower | DynamicsTrajectoryFollower | 50 | Dynamics | - |
| ProritizerDynamics | SignalPrioritizer | 25 | Prioritizer | - |
| SensorRecordStateModule | SensorRecordState | 2 | Updater | Since values are "frozen" f. current time step, logging can be placed anywhere |
| ActionLongitudinalDriverModules | ActionLongitudinalDriver | 3 | Updater | Will be expanded to ActionPrimaryDriverTasks |
| ActionSecondaryDriverTasksModules | ActionSecondaryDriverTasks | 3 | Updater | - |
| AgentUpdater | AgentUpdater | 1 | Updater | - |


\subsubsection io_input_systemconfigblueprint_channelids Channel-Ids


Channels allow components to communicate with each other.
The signalflow is set explicitly via a channel-Id of 4 digits (see also [Components and channels communication diagram](/ref dev_concepts_modulecomposition)).

The first two numbers define the sending module (XX 00).
The other two digits define the type of signal that is sent (00 XX).

Signals as well as modules can be grouped to allow explicit numbering (see tables below).

Channel-Ids between Sensor and SensorFusion are an exception to this rule. For sensor/sensor fusion communication channel-ids are 9900 + x (incremented for every new sensor)

Example:
PrioritizerAccelerationDriver -> AlgorithmLongitudinalDriver with signal of type AccelerationSignal: 1813.


**Ids for Modules (first two digits)**


Index range for module groups:

| Group             | Id      |
|-------------------|---------|
| Dynamics          | 1...10  |
| Algorithm         | 11...30 |
| DriverTasks       | 31...40 |
| Driver            | 41...50 |
| VehicleComponent  | 51...80 |
| Special           | 81...89 |
| Sensor            | 91...99 |

With corresponding defined indices :

|Module                                     |Group              |Id  |
|-------------------------------------------|-------------------|----|
| AgentUpdater                              | Dynamics          | 1  | 
| Dynamics_TrajectoryFollower               | Dynamics          | 2  | 
| Dynamics_RegularDriving                   | Dynamics          | 3  | 
| Dynamics_Collision                        | Dynamics          | 4  | 
| PrioritizerDynamics                       | Dynamics          | 5  | 
| Algorithm_LongitudinalVehicleComponent    | Algorithm         | 11 | 
| Algorithm_LongitudinalAfdm                | Algorithm         | 12 |
| Algorithm_SteeringVehicleComponent        | Algorithm         | 14 |
| Algorithm_LateralVehicleAfdm              | Algorithm         | 15 | 
| LimiterVehicleLongitudinal                | Algorithm         | 17 | 
| PrioritizerLongitudinal                   | Algorithm         | 21 | 
| PrioritizerSteering                       | Algorithm         | 22 | 
| PrioritizerAccelerationVehicleComponents  | Algorithm         | 23 | 
| PrioritizerSteeringVehicleComponents      | Algorithm         | 24 | 
| Action_LongitudinalDriver                 | DriverTasks       | 31 | 
| Action_SecondaryDriverTasks               | DriverTasks       | 32 | 
| PrioritizerTurningIndicator               | DriverTasks       | 33 | 
| AlgorithmAgentFollowingDriver             | Driver            | 41 |
| AEB                                       | VehicleComponent  | 52 | 
| ComponentController                       | Special           | 83 |
| OpenScenarioActions                       | Special           | 84 |
| Parameter_Vehicle                         | Sensor            | 92 | 
| SensorAggregation                         | Sensor            | 93 | 
| SensorFusion                              | Sensor            | 94 | 
| Sensor_Driver                             | Sensor            | 95 | 

**Ids for Signals (last two digits)**

Index range for signal groups:

| Group                 | Id      |
|-----------------------|---------|
| Dynamics              | 1...10  |
| Algorithm             | 11...30 |
| OpenScenarioActions   | 61...70 |
| Special               | 71...80 |
| Sensor                | 81...90 |
| Parameters            | 91...99 |

With corresponding defined indices :

| Signal                                 | Group                 | Id      |
|----------------------------------------|-----------------------|---------|
| Dynamics                               | Dynamics              | 01      |
| Longitudinal                           | Algorithm             | 11      |
| Steering                               | Algorithm             | 12      |
| Acceleration                           | Algorithm             | 13      |
| Lateral                                | Algorithm             | 14      |
| SecondaryDriverTasks                   | Algorithm             | 19      |
| Trajectory                             | Special               | 71      |
| AcquireGlobalPosition                  | Special               | 74      |
| CustomParameters (CustomCommandAction) | Special               | 75      |
| SpeedAction                            | Special               | 76      |
| SensorDriver                           | Sensor                | 81      |
| SensorData                             | Sensor                | 90      |
| ParametersVehicle                      | Parameters            | 92      |

\subsubsection io_input_systemconfigblueprint_paramters Parameters

For more information on the type of parameters (escpecially stochastic distributions), please refer to the [ProfilesGroup section](\ref io_input_profilescatalog_profileGroups).

**Important Note:** The syntax for defining parameters in the SystemConfigBlueprint file differs from the ProfilesCatalog syntax.
See the following example:

```xml
<parameters>
    <parameter>
        <id>StringParameter</id>
        <type>string</type>
        <unit/>
        <value>Lorem ipsum</value>
    </parameter>
    <parameter>
        <id>RandomParameter</id>
        <type>normalDistribution</type>
        <unit/>
        <value>
            <mean>15.0</mean>
            <sd>2.5</sd>
            <min>10.0</min>
            <max>20.0</max>
        </value>
    </parameter>
</parameters>
```

---

\subsection io_input_slaveconfig slaveConfig.xml

This file is necessary for the simulation to work. It describes all user configurable parameters of the experiment. 
Several parameters depend on probabilities. Each invocation then rolls for said probabilities. All probabilities need to add up to 1.0.

The slaveConfig.xml consists of the following parts:

[//]: <> (Please refer to each section!)
* [Experiment](\ref io_input_slaveconfig_experiment)
* [Scenario](\ref io_input_slaveconfig_scenario)
* [Environment](\ref io_input_slaveconfig_environment)
* [Observations](\ref io_input_slaveconfig_observations)
* [Spawners](\ref io_input_slaveconfig_spawners)

\subsubsection io_input_slaveconfig_experiment Experiment

This section contains information about the general experiment setup. These values are not specific to a single invocation.

| Tag                 | Description                                                                                      | Dependence                      |
|---------------------|--------------------------------------------------------------------------------------------------|---------------------------------|
| ExperimentId        | Id of the experiment                                                                             | Obligatory                      |
| NumberOfInvocations | Number of invocation in the experiment. For each invocation probabilities are rerolled           | Obligatory                      |
| RandomSeed          | Random seed for the entire experiment. The seed must be within the bounds of unsigned integers   | Obligatory                      |
| Libraries           | Name of the core module Libraries to use. If a name is not specified the default name is assumed | Obligatory                      |

Example:
This experiment has the id 0.
During this experiment the simulation runs 10 invocations and the first invocation starts with the random seed 5327.

```xml
<ExperimentConfig>
    <ExperimentID>0</ExperimentID>
    <NumberOfInvocations>10</NumberOfInvocations>
    <RandomSeed>5327</RandomSeed>
    <Libraries>
        <WorldLibrary>World_OSI</WorldLibrary>
    </Libraries>
</ExperimentConfig>
```

---

\subsubsection io_input_slaveconfig_scenario Scenario

This section contains information about the scenario setup for the experiment. This information does not change between invocations.

| Tag              | Description               | Dependence |
|------------------|---------------------------|------------|
| OpenScenarioFile | Name of the scenario file | Obligatory |

Example:
This experiment uses the "HighwayScenario.xosc" scenario file.
```xml
<ScenarioConfig Name="Default">
    <OpenScenarioFile>HighwayScenario.xosc</OpenScenarioFile>
</ScenarioConfig>
```

---

\subsubsection io_input_slaveconfig_environment Environment

This section contains information about the world and the general environment inside the simulation. Every invocation re-rolls the environment probabilities.
All probabilities need to add up to 1.0.

|Tag|Description|Dependence|
|---|-----------|----------|
|TimeOfDay|Number representing the time of day in hours ranging from 1-24. Currently not used.|At least one entry is required|
|VisibilityDistance|Range agents can see in m|At least one entry is required|
|Friction|Friction on the road. Used by the LimiterAccelerationVehicleComponents & DynamicsRegularDriving|At least one entry is required|
|Weather|Weather as string. Currently not used|At least one entry is required|

Example:
Every invocation has the time set to 15:00.
In 70% of all invocation drivers can see 125 meter and for the other 30% of invocations the drivers can see 250 meter.
Every invocation has a friction of 0.3.
Every invocation has sunny weather.

```xml
<EnvironmentConfig Name="Default">
    <TimeOfDays>
        <TimeOfDay Value="15" Probability="1.0"/>
    </TimeOfDays>
    <VisibilityDistances>
        <VisibilityDistance Value="125" Probability="0.7"/>
        <VisibilityDistance Value="250" Probability="0.3"/>
    </VisibilityDistances>
    <Frictions>
        <Friction Value="0.3" Probability="1.0"/>
    </Frictions>
    <Weathers>
        <Weather Value="Sunny" Probability="1.0"/>
    </Weathers>
</EnvironmentConfig>
```

---

\subsubsection io_input_slaveconfig_observations Observations

In this section all observation libraries are defined with their parameters.
A specific library is loaded by adding an entry to the `Observations` tag in the slaveConfig.xml (see example below).
The `Library` tag contains the name of the library.
The `Parameters` tag provides an optional list of keys/values.
These parameters are specific to each type of observation libraries.

```xml
<Observations>
  <Observation>
    <Library>Observation_Log</Library>
    <Parameters>
      <String Key="OutputFilename" Value="simulationOutput.xml"/>
      <Bool Key="LoggingCyclicsToCsv" Value="false"/>
      <StringVector Key="LoggingGroup_Trace" Value="XPosition,YPosition,YawAngle"/>
      <StringVector Key="LoggingGroup_Sensor" Value="Sensor*_DetectedAgents,Sensor*_VisibleAgents"/>
      <StringVector Key="LoggingGroups" Value="Trace,Sensor"/>
    </Parameters>
  </Observation>
</Observations>
```

\paragraph io_input_slaveconfig_observations_observationlog Observation_Log

This is the standard observation module, that writes the [simulationOutput.xml](\ref io_output_simout). It has the following parameters:

| Parameter           | Description                                                             |
|---------------------|-------------------------------------------------------------------------|
| OutputFilename      | Name of the output file                                                 |
| LoggingCyclicsToCsv | If true, the cyclics are written into a separate CSV file for every run |
| LoggingGroup_<NAME> | Defines which columns belong to the logging group named NAME            |
| LoggingGroups       | Defines which logging groups are active                                 |

**LoggingGroup definitions using wildcards**

In addition to specifying logging column names by stating their full names, it is allowed to use the wildcard character `*` in a `LoggingGroup_NAME` entry.
The wildcard can be used at most one time in each column reference.

For an example, please see the slaveConfig.xml snippet above.

\paragraph io_input_slaveconfig_observations_observationentities Observation_EntityRepository

This observation module writes the EntityRepository into a CSV file.  It has the following parameters:

| Parameter               | Description                                                             |
|-------------------------|-------------------------------------------------------------------------|
| FilenamePrefix          | Prefix of the output files, e.g. `XY` will result in `XY_Run_000.csv`   |
| WritePersistentEntities | Choose:<br>- Consolidated: Persistent entities are written together with non-persistent into one file per run.  <br>- Separate: Persistent entities are written once into a file `<prefix>_Persistent.csv`<br>- Skip: Persistent entities are not written |

---

\subsubsection io_input_slaveconfig_spawners Spawners

In this section the spawners are defined with their Profile (defined in the ProfilesCatalog). The same library can be loaded multiple times with different profiles.
A spawner is either of type "PreRun", meaning it is triggered only once at the start of the simulation, or "Runtime", meaning it is triggered in every timestep.
If different spawners are to be triggered at the same time the spawner with the highest priority is triggered first.

```xml
<Spawners>
  <Spawner>
    <Library>SpawnPointScenario</Library>
    <Type>PreRun</Type>
    <Priority>1</Priority>
  </Spawner>
  <Spawner>
    <Library>SpawnPointPreRunCommon</Library>
    <Type>PreRun</Type>
    <Priority>0</Priority>
    <Profile>DefaultPreRunCommon</Profile>
  </Spawner>
  <Spawner>
    <Library>SpawnPointRuntimeCommon</Library>
    <Type>Runtime</Type>
    <Priority>0</Priority>
    <Profile>DefaultRuntimeCommon</Profile>
  </Spawner>
</Spawners>
```

---

\subsection io_input_profilescatalog ProfilesCatalog.xml

The ProfilesCatalog contains all AgentProfiles, VehicleProfiles and generic ProfileGroups and Profiles. Depending on the configuration the simulator could require  a "Driver"-ProfileGroup, a "Spawner"- and "TrafficGroup"-ProfileGroup , or sensor and vehiclecomponent specific ProfileGroups.

* [AgentProfiles](\ref io_input_profilescatalog_agentprofiles)
* [VehicleProfiles](\ref io_input_profilescatalog_vehicleprofiles)
* [ProfileGroups](\ref io_input_profilescatalog_profileGroups)
* [Driver-ProfileGroup](\ref io_input_profilescatalog_driverprofiles)
* [VehicleComponent-ProfileGroups](\ref io_input_profilescatalog_vehiclecomponentprofiles)
* [TrafficGroup-ProfileGroup](\ref io_input_profilescatalog_trafficgroupprofiles)
* [Spawner-ProfileGroup](\ref io_input_profilescatalog_spawnerprofiles)

---

\subsubsection io_input_profilescatalog_agentprofiles AgentProfiles

In this section all AgentProfiles are defined. An AgentProfile is either static or dynamic. A static AgentProfile consists of a SystemConfig and a VehicleModel.
A dynamic AgentProfile specifies the composition of the agent according to certain probabilities.
Here the initial driver and the vehicle profile of an agent get specified.
At least one AgentProfile is required. But every AgentProfile referenced in the used Scenario.xosc file or [TrafficConfig](\ref io_input_slaveconfig_trafficconfig) must exist.
All probabilities must add up to 1.0.

|Tag|Description|Dependence|
|---|-----------|----------|
|DriverProfile|Name of the DriverProfile|At least one entry is required|
|VehicleProfiles|All different ADASProfiles can be referenced here. For further information see [VehicleProfiles](\ref io_input_slaveconfig_vehicleprofiles)|Optional|

Example:
In this experiment the ego agent is defined by the system with Id 0 in systemConfig.xml and the VehicleModel car_bmw_7.
Every LuxuryClassCarAgent has the driver profile "AgentFollowingDriver".
Regarding the vehicle profile 50% have a MINI Cooper and the other 50% drive a BMW 7.

```xml
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
```

---

\subsubsection io_input_profilescatalog_vehicleprofiles VehicleProfiles

This sections contains all vehicle profiles. Every VehicleProfile used by [AgentProfiles](\ref io_input_profilescatalog_agentprofiles) must be listed here.

```xml
 <VehicleProfiles>
    <VehicleProfile Name="BMW 7">
        <Model Name="car_bmw_7"/>
        <Components>
               <Component Type="AEB">
                <Profiles>
                    <Profile Name="aeb" Probability="0.5"/>
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
```

|Attribute|Description|
|---------|-----------|
|Name|Name of the vehicle profile|
|Components|Lists all ADAS and other components in the vehicle|
|Sensors|Lists all sensors in the vehicle|

\paragraph  io_input_profilescatalog_vehicleprofiles_components Components

|Attribute|Description|
|---------|-----------|
|Type|Type of the component. Must be identical to the component name in the SystemConfigBlueprint|
|Profile|All possible profiles of the component with probabilities. Probabilities do not need to add up to 1.|
|SensorLinks|Defines which sensor this component uses as input|

If the probabilities of the profiles do not add up to 1, it means that the vehicle possibly does not have this type of component

\paragraph  io_input_profilescatalog_vehicleprofiles_sensors Sensors

|Attribute|Description|
|---------|-----------|
|Id|Identifier for the sensor used by the SensorLink definition of the components|
|Position|Position of the sensor in the vehicle in relative coordinates|
|Profile|All possible profiles of the sensor with probabilities.|

---

\subsubsection io_input_profilescatalog_profileGroups ProfileGroups

A ProfileGroup defines all the possible profiles of a component.
A single profile is a set of parameters that are passed to the component in the same way as the parameters in the SystemConfig.
Note: For components that have their parameters defined in the ProfilesCatalog the parameters in the SystemConfigBlueprint are ignored.
Parameters can either be simple or stochastic.
Simple parameters only have one value, while stochastic parameters have a minimum and maximum value as well as distribution specific parameters.
Which parameters are needed/supported depends on the component.

```xml
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
```

There are the following types of simple parameters:
* Bool
* Int
* Double
* String
* IntVector
* DoubleVector
* StringVector

If a parameter is stochastic it can be defined as any to be drawn from any of the following distributions:

| Distribution            |Additional attributes                             |
|-------------------------|--------------------------------------------------|
| NormalDistribution      | (Mean and SD) or (Mu and Sigma) (equivalent)     |
| LogNormalDistributio    | (Mean and SD) or (Mu and Sigma) (not equivalent) |
| UniformDistribution     | -                                                |
| ExponentialDistribution | Lambda or Mean (Mean = 1 / Lambda)               |
| GammaDistribution       | (Mean and SD) or (Shape and Scale)               |

Additionally there is the list type.
The list contains any number of list items which itself contain a list of parameters.
Lists can be nested at most two times.

```xml
<List Name="AgentProfiles">
    <ListItem>
        <String Key="Name" Value="LuxuryClassCarAgent"/>
        <Double Key="Weight" Value="0.4"/>
    </ListItem>
    <ListItem>
        <String Key="Name" Value="MiddleClassCarAgent"/>
        <Double Key="Weight" Value="0.6"/>
    </ListItem>
</List>
```

A Profile can also reference another Profile in another ProfileGroup.
In these case the importer handles the reference as if it was substituted by all subelements of the referenced Profile.
References may not be nested.

```xml
<Reference Type="GroupType" Name="ProfileName"/>
```

---

\subsubsection io_input_profilescatalog_driverprofiles DriverProfiles

This section contains all driver profiles used by the simulation. At least one driver profile is required.

[//]: <> (Please refer to each section!)
* [AlgorithmAgentFollowingDriverModel](\ref io_input_profilescatalog_profilescatalog_agentfollowingdrivermodel)

\paragraph io_input_slaveconfig_profilescatalog_agentfollowingdrivermodel AlgorithmAgentFollowingDriverModel

This driver type adapts its velocity to an agent in front and holds a desired velocity if there's no front agent available (like adaptive cruise control). The lateral guidance always keeps the agent in the middle of the lane.

```xml
<ProfileGroup Type="Driver">
    <Profile Name="AgentFollowingDriver">
        <String Key="Type" Value="AlgorithmAgentFollowingDriverModel"/>
        <String Key="AlgorithmLateralModule" Value="AlgorithmLateralAfdm"/>
        <String Key="AlgorithmLongitudinalModule" Value="AlgorithmLongitudinalAfdm"/>
        <Double Key="VelocityWish" Value="35.0"/>
        <Double Key="Delta" Value="4.0"/>
        <Double Key="TGapWish" Value="1.5"/>
        <Double Key="MinDistance" Value="2.0"/>
        <Double Key="MaxAcceleration" Value="1.4"/>
        <Double Key="MaxDeceleration" Value="2.0"/>
    </Profile>
</ProfileGroup>
```
|Parameter|Description|Dependence|
|---------|-----------|----------|
|AlgorithmLateralModule|Sets the driver behaviour model that generates the steering wheel angle of the driver|Obligatory|
|AlgorithmLongitudinalModule|Sets the driver behaviour model that generates the accelerator and brake pedal position and the current gear of the driver|Obligatory|
|VelocityWish|Desired speed|Optional (default: 120 [km/h]/ 33.33 [m/s])|
|Delta|Free acceleration exponent characterizing how the acceleration decreases with velocity (1: linear, infinity: constant)|Optional (default: 4.0)|
|TGapWish|Desired time gap between ego and front agent|Optional (default: 1.5)|
|MinDistance|Minimum distance between ego and front (used at slow speeds); Also called jam distance|Optional (default: 2.0)|
|MaxAcceleration|Maximum acceleration in satisfactory way, not vehicle possible acceleration|Optional (default: 1.4)|
|MaxDeceleration|Desired deceleration|Optional (default: 2.0)|

---

\subsubsection io_input_profilescatalog_vehiclecomponentprofiles VehicleComponentProfiles

This sections contains all driver assistance systems and other vehicle components and their parameter sets. 
Every VehicleComponentProfile used by [AgentProfiles](\ref io_input_profilescatalog_agentprofiles) must be listed here.
Currently there are three VehicleComponents:

[//]: <> (Please refer to each section!)
* [AEB](\ref io_input_profilescatalog_vehiclecomponentprofiles_aeb)
* [DynamicsTrajectoryFollower](\ref io_input_profilescatalog_vehiclecomponentprofiles_trajectoryfollower)
* [SensorGeometric2D](\ref io_input_profilescatalog_vehiclecomponentprofiles_geometric2d)

\paragraph io_input_profilescatalog_vehiclecomponentprofiles_aeb AEB

For details about the system go to [AEB - Module](\ref dev_modules_adas_aeb).

|Attribute                             |Description                                                                            |
|--------------------------------------|---------------------------------------------------------------------------------------|
|CollisionDetectionLongitudinalBoundary|Additional length in m added the vehicle boundary when checking for collision detection|
|CollisionDetectionLateralBoundary     |Additional width in m added the vehicle boundary when checking for collision detection |
|TTC                                   |Time to collision which is used to trigger AEB in seconds                              |
|Acceleration                          |Braking acceleration when activated                                                    |

```xml
<ProfileGroup Type="AEB">
    <Profile Type="AEB" Name="AEB1">
        <Double Key="CollisionDetectionLongitudinalBoundary" Value="4.0"/>
        <Double Key="CollisionDetectionLateralBoundary" Value="1.5"/>
        <Double Key="TTC" Value="2.0"/>
        <Double Key="Acceleration" Value="-2"/>
    </Profile>
    ...
</ProfileGroup>
```

---

\paragraph io_input_profilescatalog_vehiclecomponentprofiles_trajectoryfollower DynamicsTrajectoryFollower

This vehicle component aligns the vehicle to a certain trajectory. Every vehicle component profile of this type needs a [Trajectory](\ref io_input_trajectory) file.

All attributes are required. The profile name may be chosen freely.

|Attribute|Description|
|---------|-----------|
|TrajectoryFile|The file defining the Trajectory to follow|
|AutomaticDeactivation|If true, the trajectory follower relinquishes control of the vehicle after the final instruction in the TrajectoryFile.|
|EnforceTrajectory|If true, the trajectory follower overrides external input related to the vehicle's travel.|

```xml
<ProfileGroup Type="DynamicsTrajectoryFollower">
    <Profile Name="BasicTrajectoryFollower">
        <String Key="TrajectoryFile" Value="Trajectory.xml"/>
        <Bool Key="AutomaticDeactivation" Value="true"/>
        <Bool Key="EnforceTrajectory" Value="true"/>
    </Profile>
</ProfileGroup>
```

---

\subsubsection io_input_profilescatalog_vehiclecomponentprofiles_geometric2d SensorGeometric2D

|Parameter|Description|
|---------|-----------|
|LatencyMean|Mean value of the latency|
|LatencySD|Standard deviation of the latency|
|LatencyMin|Minimum value of the latency|
|LatencyMax|Maximum value of the latency|
|FailureProbability|How likely the sensor fails to detected an object|
|DetectionRange|Range in which the sensor can detect objects|
|OpeningAngleH|Angle of the circular sector|
|EnableVisualObstruction|Wether this sensor uses [visual obstruction](\ref dev_agent_modules_geometric2d_obstruction)|
|RequiredPercentageOfVisibleArea|Fraction of the area of an object that has to be in the sensor detection field in order for being detected|Geometric2D|

```xml
<ProfileGroup Type="Geometric2D">
    <Profile Name="Standard">
        <Double Key="OpeningAngleH" Value="0.35"/>
        <Double Key="DetectionRange" Value="300"/>
        <Double Key="LatencyMean" Value="0"/>
        <Double Key="LatencySD" Value="0"/>
        <Double Key="LatencyMin" Value="0"/>
        <Double Key="LatencyMax" Value="0"/>
        <Double Key="FailureProbability" Value="0"/>
        <Bool Key="EnableVisualObstruction" Value="false"/>
        <Double Key="RequiredPercentageOfVisibleArea" Value="0.001" />
    </Profile>
</ProfileGroup>
```

---

\subsection io_input_scenario Scenario.xosc

This file is necessary for the simulation to work. It describes the initial setup of the scenario.

For further information see the [Scenario.xosc documentation](\ref scenario).


---

\subsection io_input_scenery SceneryConfiguration.xodr
This file is necessary for the simulation to work. It describes the road network and the objects of the road for the simulation run. The file is structured according to the OpenDRIVE standard.
The file name can be adjusted as long as it's referred to in the [ScenarioConfig](\ref io_input_slaveconfig_scenarioconfig) of the slaveConfig.xml. But the Visualization can only load files called "SceneryConfiguration.xodr".
If the objects are not openDrive conform the simulation is aborted. The simulator has some additional requirements on top of the openDrive standard, if objects don´t meet the requirements of the simulator they will be ingored.

Those requirements are:

* length must be greater than 0.
* width must be greater than 0.
* radius must be 0.

---

\subsection io_input_vehiclemodels VehicleModelsCatalog.xosc

This file is necessary for the simulation to work. It contains all currently available vehicle models for the simulation. For each vehicle the physical parameters are stored here.
The name of this file needs has to be specified in the scenario configuration file (see \ref io_input_slaveconfig_scenarioconfig).

\subsection io_input_pedestrianmodels PedestrianModelsCatalog.xosc

This file is necessary for the simulation to work. It contains all currently available pedestrian models for the simulation. For each pedestrian the physical parameters are stored here.
The name of this file needs has to be specified in the scenario configuration file (see \ref io_input_slaveconfig_scenarioconfig).

\note Currently, pedestrian models are internally handled the same way as vehicle models. Vehicle specific parameters are set to their default values.

---

\subsection io_input_systemconfig systemConfig.xml
SystemConfig files are optional.
They discribe static configurations of agents and are therefore an alternative to the dynamic sampling of an agent during runtime.
The schema is the same as for the [SystemConfigBlueprint](\ref io_input_systemconfigblueprint).

\section io_output Output

In this section the simulation output files and their contents are described.

[//]: <> (Please refer to each section!)
1. [simulationOutput.xml](\ref io_output_simout)
1. [LogSlave.txt](\ref io_output_logslave)

\subsection io_output_simout simulationOutput.xml
Every successful run generates a simulation output. The output can contain multiple invocations of the same configuration with different random seeds. For each invocation a RunResult is stored, which contains information about the agents and their parameters. As well as run specific events and parameters.

```xml
<SimulationOutput>
    <RunResults>
        <RunResult RunId="0">
            ...
        </RunResult>
        <RunResult RunId="1">
            ...
        </RunResult>
    </RunResults>
</SimulationOutput>
```

The RunResult consist out of the following parts:

[//]: <> (Please refer to each section!)
* [RunStatistics](\ref io_output_simout_runstatistics)
* [Events](\ref io_output_simout_events)
* [Agents](\ref io_output_simout_agents)
* [Cyclics](\ref io_output_simout_cyclics)

\subsubsection io_output_simout_runstatistics RunStatistics

This section contains the RandomSeed and general statistics of the invocation.

|Tag|Description|
|---|-----------|
|RandomSeed|Random seed used for this invocation|
|VisibilityDistance|Visibility distance of the world|
|StopReason|Displays the reason why the simulation stopped. Currently only due to time out|
|StopTime|Currently not used and set to -1|
|EgoAccident|Flag which shows whether the ego agent was involved in an accident|
|TotalDistanceTraveled|Total traveled distance of all agents|
|EgoDistanceTraveled|Total traveled distance of ego vehicle only|

\subsubsection io_output_simout_events Events

This section contains all events that occurred during the invocation.
Event can either be triggered by an EventDetector, Manipulator or by certain vehicle components.
They are used to track special behavior in the simulation.

| Attribute          | Description                                                                                                                                                                                |
|--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Time               | Time in ms when the event occured.                                                                                                                                                         |
| Source             | Name of the component which created the event. `OpenSCENARIO`, if triggered by an OpenSCENARIO condition.                                                                                  |
| Name               | In case of an OpenSCENARIO event, a path expression trough Story, Act, Sequence, Maneuver and Event (as described in OpenSCENARIO file). Otherwise determined by the triggering component. |
| TriggeringEntities | List of entity IDs triggering this event.                                                                                                                                                  |
| AffectedEntities   | List of entity IDs affected by this event.                                                                                                                                                 |
| Parameters         | List of generic key/value string pairs.                                                                                                                                                    |

\subsubsection io_output_simout_agents Agents

This section contains some information on how each agent is configured.

|Attribute|Description|
|---------|-----------|
|Id|Identification number|
|AgentTypeGroupName|The agent category. This can either be Ego, Scenario or Common|
|AgentTypeName|Name of the agent profile|
|VehicleModelType|Name of the vehicle model|
|DriverProfileName|Name of the driver profile|


```xml
<Agent Id="0" AgentTypeGroupName="Ego" AgentTypeName="MiddleClassCarAgent" VehicleModelType="car_bmw_7" DriverProfileName="Regular">
```

The VehicleAttributes tag lists basic information of the vehicle parameters.

|Attribute|Description|
|---------|-----------|
|Width|Width of the vehicles bounding box|
|Length|Length of the vehicles bounding box|
|Height|Height of the vehicles bounding box|
|LongitudinalPivotOffset|Distance between the center of the bounding box and the reference point of the agent. Positive distances are closer to the front and Negative distances are closer to the rear of the vehicle.|

The Sensors tag lists all sensors of the agent and their parameters.

|Attribute|Description|
|---------|-----------|
|Name|Name of the component|
|Model|Type of the sensor|
|MountingPosLongitudinal|Relative longitudinal position of the sensor in relation to the agent|
|MountingPosLateral|Relative lateral position of the sensor in relation to the agent|
|MountingPosHeight|Relative height of the sensor in relation to the agent|
|OrientationPitch|Pitch rotation of the sensor in relation to the agent|
|OrientationYaw|Yaw rotation of the sensor in relation to the agent|
|OpeningAngleH|Horizontal opening angle of the sensor|
|OpeningAngleV|Vertical opening angle of the sensor|
|DetectionRange|Range how far the sensor reaches in m|

Note: Calculation of visual obstruction is currently supported by the Geometric2D sensor. See [file](systemConfigBlueprint.xml) for configuration.

```xml
<Sensor Name="Geometric2DFront" Model="Geometric2D" MountingPosLongitudinal="0" MountingPosLateral="0" MountingPosHeight="0.5" OrientationPitch="0" OrientationYaw="0" OpeningAngleH="20" OpeningAngleV="-999" DetectionRange="300"/>
```

\subsubsection io_output_simout_cyclics Cyclics

This section contains all logged parameters of the agents per timestep. The Header tag defines the layout of all samples. Each entry of the Header tag consists of the agent id and the name of the logged value. A Sample is contains all information for one specific time step. The values in Sample are ordered according to the definition in Header. If a value is undefined, because an agent did not exist yet or got removed the value is ' '.

Example:
In this example exist two agents with the ids 0 and 1.
Two timesteps were being tracked, one at 0 seconds and one at 100 ms.
Agent 0 has a constant velocity of 30 m/s and starts at the position X: 100 and Y: 50.
Agent 1 has a initial velocity of 40 m/s and starts at the position X: 200 and Y: 50.

```xml
<Cyclics>
    <Header>00:VelocityEgo, 00:XPosition, 00:YPosition, 00:YawAngle, 01:VelocityEgo, 01:XPosition, 01:YPosition, 01:YawAngle</Header>
    <Samples>
        <Sample Time="0">30, 100, 50, 0, 40, 200, 50, 0</Sample>
        <Sample Time="100">30, 103, 50, 0, 40, 204, 50, 0</Sample>
    </Samples>
</Cyclics>
```

\subsection io_output_logslave LogSlave.txt

In this log file errors and warnings of the slave are noted. In a run without any issues this file should be empty.

