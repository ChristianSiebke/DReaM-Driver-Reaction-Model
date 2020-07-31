\page dev_agent_modules Agent Modules

\tableofcontents

[//]: <> (Please sort by alphabet.)

The modules of openPASS are split the two major categories `Framework Modules` and `Agent Modules` and defined within [SystemConfigBlueprint](\ref io_input_systemconfigblueprint).
`Framework Modules` are instantiated once for the entire simulation  and **all of them are necessary**.
`Agent Modules` are instantiated individually for each agent and as such, it depends on the agents configuration which are necessary.

This section covers the **Agent Modules**, the **Framework Modules** can be found [here](@ref dev_framework_modules).

\section dev_agent_modules_actionLongitudinalDriver Action_LongitudinalDriver

Updates the agents pedal positions and gear.
The input for this module is prepared by the AlgorithmLongitudinal Module.

---

\section dev_agent_modules_actionSecondaryDriver Action_SecondaryDriver

Updates the agents BrakingLight, Indicator, Horn and all Lightswitches (Headlight, HighBeam, Flasher).
The input for this module is prepared by the driver module.

---

\section dev_agent_modules_adas Advanced Driver Assistance System

The simulator currently has multiple advanced driver assistance systems.
For details see here: [ADAS](\ref adas)

---

\section dev_agent_modules_agentFollowingDriver AgentFollowingDriverModel

The AgentFollowingDriver is a driver module, which tries to follow the agent in front of it.
It only adjusts the acceleration to prevent crashing into the previous agent. This module does not change the lane.
If there is no agent in front of it the module tries to drive with a velocity of 120 km/h. 

---

\section dev_agent_modules_agentUpdater AgentUpdater

The AgentUpdater executes all Set-Methods of the agent dynamics after the DynamicsPrioritizer. This includes position, velocity, acceleration and rotation.

---

\section dev_agent_modules_algorithmLaterDriver Algorithm_Lateral

This module converts the lateral input of the driver module into a steeringwheel angle.

---

\section dev_agent_modules_algorithmLongitudinalDriver Algorithm_Longitudinal

This module converts the acceleration input of the driver module into pedal positions and gear.

---

\section dev_agent_modules_dynamicsTrajectoryFollower Dynamics_TrajectoryFollower

This module forces agents to drive according to a specific trajectory. The trajectory is defined in the scenario. This module is disabled by default and is activated if a trajectory from openSCENARIO is triggered.
It is always important that the trajectories matches the current scenery file, otherwise the Agent could be placed outside of valid lanes. If the agent gets placed on a invalid position, it will be deleted.

The following attributes must be defined for these module:

|Attribute|Type|Description|
|---------|----|-----------|
|AutomaticDeactivation|Bool|If true, the trajectory follower relinquishes control of the vehicle after the final instruction in the TrajectoryFile.|
|EnforceTrajectory|Bool|If true, the trajectory follower overrides external input related to the vehicle's travel.|

Examples:

An example VehicleComponentProfile for the TrajectoryFollower defining these attributes would look as follows:

```xml
<ProfileGroup Type="Dynamics_TrajectoryFollower">
    <Profile Type="DynamicsTrajectoryFollower" Name="BasicTrajectoryFollower">
        <Bool Key="AutomaticDeactivation" Value="true"/>
        <Bool Key="EnforceTrajectory" Value="true"/>
    </Profile>
</ProfileGroup>
```

An example of a Scenario Sequence to activate the TrajectoryFollower would look as shown below.

```xml
<ManeuverGroup name="TrajectorySequence" maximumExecutionCount="1">
    <Actors selectTriggeringEntities="false">
        <EntityRef entityRef="TF"/>
    </Actors>
    <Maneuver name="StateChangeManeuver">
        <Event name="StateChangeEvent" priority="overwrite">
            <Action name="ComponentStateChangeManipulator">
                <PrivateAction>
                    <RoutingAction>
                        <FollowTrajectoryAction>
                            <Trajectory name="LaneChange" closed="false">
                                <Shape>
                                    <Polyline>
                                        <Vertex time="0">
                                            <Position>
                                                <WorldPosition x="76.17" y="5.625" z="0" h="0.0" p="0" r="0"/>
                                            </Position>
                                        </Vertex>
                                        <Vertex time="0.1">
                                            <Position>
                                                <WorldPosition x="79.17" y="5.62485" z="0" h="-0.00005" p="0" r="0"/>
                                            </Position>
                                        </Vertex>
                                        <Vertex time="0.2">
                                            <Position>
                                                <WorldPosition x="82.17" y="5.6247" z="0" h="-0.00005" p="0" r="0"/>
                                            </Position>
                                        </Vertex>
                                    </Polyline>
                                </Shape>
                            </Trajectory>
                        </FollowTrajectoryAction>
                    </RoutingAction>
                </PrivateAction>
            </Action>
            <StartConditions>
                <ConditionGroup>
                    <Condition name="Conditional">
                        <ByValue>
                            <SimulationTime value="0.0" rule="greater_than"/>
                        </ByValue>
                    </Condition>
                </ConditionGroup>
            </StartConditions>
        </Event>
    </Maneuver>
</Sequence>
```

---

\section dev_agent_modules_dynamicsLateral Dynamics_Collision

If the number of collision partners of the agent is bigger than in the previous timestep, the DynamicsCollision module calculates the collision. 
Currently the collision is implemented fully inelastic, i.e. all agents will have the same velocity after the collision, while the momentum is conserved. 
After the collision the agents slow down with a fixed deceleration until fully stopped.

---

\section dev_agent_modules_dynamicsRegularDriving Dynamics_RegularDriving

The module takes care that the motion of the agent fit to the physical limitations, such as friction or maximum possible acceleration based on the current gear.
This module uses both the world friction and the vehiclemodelparameter friction.
Thereby it calculates the dynamics of the agents in every time step.
The currently covered  dynamics are _Acceleration_, _Velocity_, and consequently _Position_, _Yaw angle_ and _Yaw rate_.
The input for this module is the steeringwheel angle and the new acceleration of the vehicle.

---

\section dev_agent_modules_fmuwrapper FMU wrapper

The FMU Wrapper provides a connection to arbitrary FMUs (Functional Mock-up Unit).
A FMU has to be compatible with the FMI 1.0 or FMI 2.0 specification (Functional Mock-up Interface) and has to be ABI (Application Binary Interface) compatible with the OpenPassSlave binary.

Additional reading about FMI is provided by the FMI standard website at https://fmi-standard.org/.
For interfacing the FMUs in openPASS, the Modelon FMI Library is used, which is recommended on the FMI standard website. See https://jmodelica.org/.

\subsection dev_agent_modules_fmuwrapper_fmupackage FMU package format

FMI defines a packaging format for FMUs. The used container format is ZIP.
It basically contains - among other parts -  the compiled FMU code (as *.dll or *.so, depending on the platform) and the `modelDescription.xml`.
Latter provides meta-data about the FMU, i. e.
- Author information
- Model name, identifier and description
- Generation timestamp
- Name and datatype of model variables (inputs and outputs)


\subsection dev_agent_modules_fmuwrapper_arch Architectural overview

The wrapper is instantiated as a component of an agent.
It instantiates an implementation of the FmuHandlerInterface, which is a specialization depending on the type of the FMU.
Currently the only such FmuHandler is the OsmpFmuHandler

\subsection dev_agent_modules_fmuwrapper_config Configuration

Some Parameter are always required for the FmuWrapper. Depending on the FmuHandler additional parameters may be needed.

```xml
<ProfileGroup Type="FMU1">
    <Profile Name="OSMP">
        <String Key="FmuPath" Value="resources/OSMP.fmu"/>
        <Bool Key="Logging" Value="true"/>
        <Bool Key="CsvOutput" Value="true"/>
        <Bool Key="UnzipOncePerInstance" Value="false"/>
        <String Key="FmuType" Value="OSMP"/>
    </Profile>
</ProfileGroup>
```

| **Key**              | **Type** | **Description**                                                                                                                  |
|----------------------|----------|----------------------------------------------------------------------------------------------------------------------------------|
| FmuPath              | string   | Path to FMU file. Relative and absolute paths are supported.                                                                     |
| Logging              | bool     | If set to true, FMU initialization and execution task are logged to a text file.                                                 |
| CsvOutput            | bool     | If set to true, FMI outputs are logged to a CSV file.                                                                            |
| UnzipOncePerInstance | bool     | If set to true, unpack the FMU once for each running instance, instead of once per FMU file (forced to `true` on Linux systems)  |
| FmuType              | string   | Type of the FMU (currently only "OSMP" supported")                                                                               |

Upon instantiation of the FMU wrapper, it will extract the FMU ZIP file to a subfolder "Unzip", located at the same path as the FMU file itself.
Then the `modelDescription.xml` is parsed and the FMU is checked for compatibility.

If `CsvOutput` is set to `true`, a subfolder "Output" will be created in the path of the FMU file.
FMI output data will be logged to files inside this directory, with the agent id appended in the filename.
This output can then be used for visualization in a spreadsheet application or it may be processed in any other way.

Same goes for `Logging` with the folder "Log".

If `UnzipOncePerInstance` is set to `true`, an integer number will be appended to these subdirectories names.

\subsection dev_agent_modules_fmuwrapper_osmp OSMP FMU

OSMP is currently the only supported type of FMU.
It allows the pass input to the FMU as OSI messages as well as recieve output as OSI message.
For more information on OSMP see https://github.com/OpenSimulationInterface/osi-sensor-model-packaging.

The OsmpFmuHandler has the following additional (optional) parameters:

| **Key**                   | **Type** | **Description**                                                                                 |
|---------------------------|----------|-------------------------------------------------------------------------------------------------|
| Input_SensorView          | string   | Name of the FMU variable to which the SensorView is sent                                        |
| Input_TrafficCommand      | string   | Name of the FMU variable to which the TrafficCommand is sent                                    |
| Init_Groundtruth          | string   | Name of the FMU variable to which the Groundtruth is sent during the initialization             |
| Output_SensorData         | string   | Name of the FMU variable from where a SensorData should be recieved                             |
| Output_TrafficUpdate      | string   | Name of the FMU variable from where a TrafficUpdate should be recieved                          |
| WriteSensorViewOutput     | bool     | If true the SensorView is written into a JSON file                                              |
| WriteTrafficCommandOutput | bool     | If true the TrafficCommand is written into a JSON file                                          |
| WriteGroundtruthOutput    | bool     | If true the Groundtruth is written into a JSON file                                             |
| WriteSensorDataOutput     | bool     | If true the SensorData is written into a JSON file                                              |
| WriteTrafficUpdateOutput  | bool     | If true the TrafficUpdate is written into a JSON file                                           |
| EnforceDoubleBuffering    | bool     | If true the wrapper will throw an error if FMU doesn't use double buffering. Defaults to false. |

The type of OSI messages the OsmpFmuHandler sends an recieves is defined by its parameters. Only messages for which a FMU variable is given in the configuration are sent/recieved.
An additional parameter defines whether the message should be logged as JSON file for every agent and every timestep (see table above).

Currently these messages are supported:

* SensorView: SensorView generated from the Groundtruth with this agent is host vehicle
* TrafficCommand: Trajectory from openSCENARIO, that will be converted into a TrafficCommand
* SensorData: Output of a sensor, that can be send to other modules as SensorDataSignal
* TrafficUpdate: Will be converted to a DynamicsSignal

---

\section dev_agent_modules_limiter LimiterAccelerationVehicleComponents
This module limits the AccelerationSignal from the PrioritizerAccelerationVehicleComponents to the constraints given by the vehicle. The [DynamicsTrajectoryFollower](\ref dev_agent_modules_dynamicsTrajectoryFollower) can then use this signal to calculate a trajectory.

The limit is calculated by a<SUB>lim</SUB> = ( F<SUB>wheel</SUB> - F<SUB>roll</SUB> - F<SUB>air</SUB> ) / m<SUB>veh</SUB>, where the symbols meanings are:

| Symbol            | Decription                                     |
|:------------------|:-----------------------------------------------|
| a<SUB>lim</SUB>   | Resulting acceleration limit [m/s<SUP>2</SUP>] |
| F<SUB>wheel</SUB> | Force at wheel (provided by drivetrain) [N]    |
| F<SUB>roll</SUB>  | Force resulting from rolling resistance [N]    |
| F<SUB>air</SUB>   | Force resulting from air drag [N]              |
| m<SUB>veh</SUB>   | Mass of the vehicle [kg]                       |

The components are calculated as follows:

**Driving force**

F<SUB>wheel</SUB> = T<SUB>engine</SUB> * r<SUB>axle</SUB> / r<SUB>wheel</SUB>

| Symbol             | Decription                                                                                 |
|:-------------------|:-------------------------------------------------------------------------------------------|
| T<SUB>engine</SUB> | Resulting torque from drivetrain at current velocity (assumung best gearing selected) [Nm] |
| r<SUB>axle</SUB>   | Axle transmission ratio [1]                                                                |
| r<SUB>wheel</SUB>  | Static radius of the wheels [m]                                                            |

The engine torque T<SUB>engine</SUB> is calculated by a simple model, where the torque scales proportional with the current engine speed between 1350 and 5000 rpm, up to maximum engine torque.
From minimum engine speed up to 1350 rpm the torque scales proportional with the engine speed up to half the maximum torque.
From 5000 rpm up to maximum engine speed, the torque scales with 5000 / maxEngineSpeed, up to maximum torque.

**Rolling resistance**

F<SUB>roll</SUB> = m<SUB>veh</SUB> * c<SUB>fric</SUB> * 1g

| Symbol             | Decription                                        |
|:-------------------|:--------------------------------------------------|
| m<SUB>veh</SUB>    | Mass of the vehicle [kg]                          |
| c<SUB>fric</SUB>   | Rolling friction coefficient (constant 0.015) [1] |


**Air drag**

F<SUB>air</SUB> = rho<SUB>air</SUB> / 2 * A<SUB>front</SUB> * c<SUB>w</SUB> * v<SUP>2</SUP>

| Symbol            | Decription                                 |
|:------------------|:-------------------------------------------|
| rho<SUB>air</SUB> | Densitiy of air [kg/m<SUP>3</SUP>]         |
| A<SUB>front</SUB> | Vehicle front surface area [m<SUP>2</SUP>] |
| c<SUB>w</SUB>     | Drag coefficient [1]                       |
| v                 | Vehicle's current velocity [m/s]           |

---

\section dev_agent_modules_oscActions OpenScenarioActions

As defined by openSCENARIO, OpenScenarioActions is the relaying module for: 
- Trajectory-actions
- LaneChange-actions 
- CustomLaneChange-actions 
- UserDefined-actions.
  
If a 
- TrajectoryManipulator
- LaneChangeManipulator
- CustomLaneChangeManipulator or a
- GazeFollowerManipulator 
  
raises such an event for the specified agent, the module forwards it as signal to all interested module of the corresponding agent. The modules can than react on the signals content without time delay.

---

\section dev_agent_modules_parametersVehicle Parameters_Vehicle

The ParametersVehicle module forwards the VehicleModelParamters to all other moduls that need them via the ParametersVehicleSignal

---

\section dev_agent_modules_sensorDriver Sensor_Driver

The SensorDriver performs queries on the AgentInterface to gather information about the own agent and its surroundings. These are forwarded to the driver modules and the Algorithm modules, which use them for their calculations.

---

\section dev_agent_modules_sensorOSI Sensor_OSI

This module is a representation of various sensors and uses OSI for its input and output.
Currently it only has one type of sensor (SensorGeometric2D) which detects all objects in sector with specified range and opening angle.
The input of the sensor is a OSI SensorView generated by the OSI World and its output is a OSI SensorData structure.

For generation of the SensorView, a SensorViewConfiguration has to be provided by the sensor to the OSI World. See image for a visualization of the
dataflow.

![OSI SensorView dataflow](SensorView_Dataflow.svg)

From OSI development perspective, the OSI World would have to send back a SensorViewConfiguration to the sensor, with the contents describing the
actual configuration of the SensorView (since the World is maybe notable to provide the requested information).
As we have full control over the simulation environment, this back-channel is skipped and SensorView according to the sensor'S SensorView configuration
will always be provided.

To test whether an object is inside our sector we check
1. if it is inside the circle around the sensor with radius the detection range and
2. if it intersects a suitable polygon

Depending on the opening-angle the polygon in 2) has either four (angle < 180°) or five corners (angle >= 180°).

![four-corner kite polygon for angle lower than 180°](Sensor2D_kite_polygon.png)

| Object | intersects circle | intersects polygon | detected |
|--------|-------------------|--------------------|----------|
| A      | true              | false              | false    |
| B      | true              | true               | true     |
| C      | false             | true               | false    |
| D      | false             | false              | false    |
| E      | true              | true               | true     |

![five-corner polygon for angle greater or equal 180° and greater 360°](Sensor2D_five_corner_polygon.png)

| Object | intersects circle | intersects polygon | detected |
|--------|-------------------|--------------------|--------- |
| A      | false             | true               | false    |
| B      | true              | true               | true     |
| C      | true              | true               | true     |
| D      | true              | false              | false    |
| E      | false             | false              | false    |

For convex BBoxes the above will give correct detection results.

Both polygons are constructed from corner-points consisting out of the intersection between the opening-angle boundaries at maximum detection range and their corresponding tangents.

\subsection dev_agent_modules_geometric2d_function Function

1. Construct the polygon based on the opening-angle
2. Check if detection-field (polygon) intersects with any BBox (object-detection)
3. Calculate the distance between sensor and object
4. if (dist <= range && isIntersecting) -> object is in circular sector (object validation)

\subsection dev_agent_modules_geometric2d_cases Cases

- For angles < 1.0 * pi a four-corner (kite) polygon can be constructed out of two radiuses and two tangents.
- For angles > = 1.0 * pi and < 2.0 * pi a five-corner polygon can be constructed of two radiusas an three tangents.
- For opening-angle of exactly 2.0 * pi the distance information suffices. No polygon is needed.

\subsection dev_agent_modules_geometric2d_obstruction Visual Obstruction

Objects in front of others block the sensors line of sight. If an object is large enough it might visually obstruct others.

To check if one or multiple objects in combination "shadow" other objects the EnableVisualObstruction - flag can be set.

Also the minimum required percentage of the visible area of an object to be detected can be specified.

The implemented algorithm uses the concept of shadow-casting.

The sensor can be pictured as a light source. Every object in the detection field of the sensor will therefore cast a shadow and reduce the overall detection area behind it.

If an object is shadowed too much (e.g. the visible area is below a specified threshold) it is removed from the list of DetectedObject's and considered undetected.

![Example of shadow-casting](ShadowCasting.svg)


As depicted in the figure above, inside the initial bright area (approximated as ciruclar sector) the following steps are taken:
1. calculate the shadow polygon of each object inside the detection field and remove the casted shadow from the bright area
2. check for each object the remaining area inside the bright area polygon. If (covered object area / total object area) < threshold  - remove object from the list of detected objects.

After all shadows are removed the "real" detection field polygon (yellow area) remains.
Objects in green and blue are detected. 
The red object is completly covered by shadows and therefore not detected.

---

\section dev_agent_modules_sensorFusionOSI SensorFusionOSI

The SensorFusionOSI module allows unsorted aggregation of any data provided by sensors. All sampled detected objects can then be broadcasted to connected ADAS.

It collects all SensorDataSignals and merges them into a single SensorDataSignal.

---

\section dev_agent_modules_signalprioritizer SignalPrioritizer

All channels can only have one source. If one modul can have the same input type from multiple sources a prioritizer modul is needed in between. All sources then get an output channel to the prioritizer modul and the prioritizer gets an output to the modul, which uses this signal.
If more than an component sends an active signal during the same timestep, the prioritizer forwards only the signal from the input channel with the highest priority. These priorities are set as parameters in the systemConfigBlueprint.xml:

 ```xml
    <Component Name="[Name of the SignalPrioritizer]" Init="false" Priority="[Priority]" OffsetTime="0" ResponseTime="0" CycleTime="[CycleTime]" Library="SignalPrioritizer">
        <ComponentInputs>
            <ComponentInput Id="0" ChannelRef="[InputChannel Id 1]" />
            <ComponentInput Id="1" ChannelRef="[InputChannel Id 2]" />
            <ComponentInput Id="2" ChannelRef="[InputChannel Id 3]" />
            [...]
        </ComponentInputs>
        <ComponentOutputs>
            <ComponentOutput Id="0" ChannelRef="[OutputChannel Id]" />
        </ComponentOutputs>
        <ComponentParameterSets>
             <ComponentParameterSet Name="Regular">
                <Int Key="0" Value="[Priority sender 1]"/>
                <Int Key="1" Value="[Priority sender 2]"/>
                <Int Key="2" Value="[Priority sender 3]"/>
                [...]
            </ComponentParameterSet>
        </ComponentParameterSets>
        <ComponentObservations/>
    </Component>
 ```


| **Parameter** | **Description** |
| ------------- |----------------|
| [Name of the SignalPrioritizer]  | unique name, ideally ends with "...SignalPrioritizer" |
| [Priority] | The priority has to be between all input and output components. The higher the priority, the earlier will this fusion module be executed. e.g. inputA = 150, inputB = 200; 100 <= outputs → 100 < Prioritizer < 150; |
| [InputChannel Id] | Specify all input channels |
| [OutputChannel Id] | Specify output channel |
| [Priority sender n] | Gives the priority of the signal from sender n. The higher the value, then the more important the sender. |

One prioritizer modul can only handle signals of the same type. If there is no signal in one timestep, then the signal of the previos timestep is hold.

3. Adapt file AgentSampler.h
    * add name of the new SignalPrioritizer to list of vehicleComponentNames

\subsection dev_agent_modules_signalprioritizer_modules Existing prioritizer modules

* PrioritizerAccelerationVehicleComponents
* PrioritizerSteeringVehicleComponents
* PrioritizerLongitudinal
* PrioritizerSteering
* DynamicsPrioritizer

Also refer to the [Components and channels communication diagram](\ref dev_concepts_modulecomposition).

---

\section dev_agent_modules_cc ComponentController

\subsection dev_agent_modules_cc_overview Overview

The ComponentControoler (CC) is used to configure and handle dependencies between other vehicle components.

Example use cases could be:
- Cruise control:
    - driver requesting higher acceleration than cruise control overrides the latter
    - driver braking deactivates cruise control
- Lane keeping assistant:
    - cannot be activated by driver, if emergency braking is currently active
    - stays active, when emergency braking occours (i. e. by other ADAS)

The responsibilies of the CC are:

- Handling of all dependencies between `VehicleComponents` in case a component wants to activate
- Make information about driver, `TrajectoryFollower` and other `VehicleComponents` available to each other
- Determine the highest allowed activation state of a component and notify the affected component about this state

To achieve this tasks, each component is assigned a maximum allowed state in each timestep. This state is of type ComponentState,
which defines `Disabled`, `Armed` or `Active` as allowed states.
Drivers can be in a state of either `Active` or `Passive`.

\subsection dev_agent_modules_cc_statevc State handling inside Vehicle Component

Within a vehicle component, the information flow should be implemented as follows:

1. The vehicle component retrieves the information of other components and the current maximum allowed state from the CC.
   Other components include drivers, trajectory followers and all other vehicle components connected to the CC.
2. Based on that information the vehicle component determines its current desired state.
3. The desired state is sent to the CC.

The CC handles all the dependencies between different components and determines the maximum allowed state for each component based
on the configuration of the CC.

\subsection dev_agent_modules_cc_signals Used signals

The CC communicates with the controlled components via framework signals. An overview of the connections and signal flow is shown below.

![Vehicle components signal flow](DynamicsModules.png)

Inputs to the ComponentController:

| Source             | Contents                                              | Signal                      |
|--------------------|-------------------------------------------------------|-----------------------------|
| TrajectoryFollower | Current state                                         | ComponentStateSignal        |
| Driver             | Current state, pedal activity                         | DriverStateSignal           |
| VehicleComponent   | Current state, desired state, generic ADAS parameters | VehicleCompToCompCtrlSignal |


Output to other components:

| Destination        | Contents                                                                   | Signal                      |
|--------------------|----------------------------------------------------------------------------|-----------------------------|
| TrajectoryFollower | Current max. reachable state                                               | ComponentStateSignal        |
| Driver             | List of all ADAS with names, stati and types                               | AdasStateSignal             |
| VehicleComponent   | Current max. reachable state, list of all ADAS with names, stati and types | CompCtrlToVehicleCompSignal |

