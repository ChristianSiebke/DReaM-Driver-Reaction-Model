\page scenario Scenario Configuration

\tableofcontents

\section scenario_overview Overview

The scenario configuration file describes all dynamic configuration of a simulation run, i.e. the position of an agent as well as conditional events that alter the behaviour of the simulation during the run.
It adheres to the OpenSCENARIO standard, although we support only a subset of the features of the standard.
The different parts of the scenario configuration are described in the following chapters:

\section scenario_parameterDeclaration ParameterDeclaration

In the ParameterDeclarations section parameters of different type may be declared, that are later referenced by their name prefixed with "$".
The type can be "string", "integer" or "double". In case of "string" the value may be empty.
The parameter "OP_OSC_SchemaVersion" is mandatory. It is used to check if the schema of the scenario is supported by this version of OpenPASS.

Example
```xml
  <ParameterDeclarations>
    <ParameterDeclaration name="OP_OSC_SchemaVersion" parameterType="string" value="0.4.0"/>
    <ParameterDeclaration name="EgoSCoordinate" type="double" value="50.0"/>
  </ParameterDeclarations>
```

Example use of a parameter
```xml
    <LanePosition roadId="1" s="$EgoSCoordinate" laneId="-2" offset="0.0">
```

\section scenario_catalogs Catalogs

The <CatalogsLocations> tag defines references to various other files that describe sub features of OpenSCENARIO.
We currently support only three of these references: the [VehicleCatalog](\ref io_input_vehiclemodels), which defines the vehicle models, and the [PedestrianCatalog](\ref io_input_pedestrianmodels), which defines the pedestrian models.
For the FollowTrajectory action one can also use a TrajectoryCatalog.

Example
```xml
    <CatalogsLocations>
        <VehicleCatalog>
            <Directory path="VehicleModelsCatalog.xosc"/>
        </VehicleCatalog>
        <PedestrianCatalog>
            <Directory path="PedestrianModelsCatalog.xosc"/>
        </PedestrianCatalog>
        <PedestrianControllerCatalog>
            <Directory path=""/>
        </PedestrianControllerCatalog>
        <DriverCatalog>
            <Directory path=""/>
        </DriverCatalog>
        <ManeuverCatalog>
            <Directory path=""/>
        </ManeuverCatalog>
        <MiscObjectCatalog>
            <Directory path=""/>
        </MiscObjectCatalog>
        <EnvironmentCatalog>
            <Directory path=""/>
        </EnvironmentCatalog>
        <TrajectoryCatalog>
            <Directory path=""/>
        </TrajectoryCatalog>
        <RouteCatalog>
            <Directory path=""/>
        </RouteCatalog>
    </CatalogLocations>
```

\section scenario_roadnetwork RoadNetwork

The <RoadNetwork> tag contains the reference to the OpenDrive [SceneryConfiguration](\ref io_input_scenery).

Example
```xml
    <RoadNetwork>
        <LogicFile filepath="SceneryConfiguration.xodr"/>
        <SceneGraphFile filepath=""/>
    </RoadNetwork>
```

\section scenario_entities Entities

The <Entities> tag defines all agents that are present at the start of the simulation at a predefined position.
There may be any arbitrary number of Scenario Agents (zero Scenario Agents is valid).
Each agent is described by a name and a reference to a vehicle profile in the [ProfilesCatalog](\ref io_input_profilescatalog) (Important Note: This deviates from the OpenSCENARIO standard).
To specify a Scenario Agent as the Ego Agent, the Agent must be named "Ego".
Entities can also be grouped into selections.
The selection called "ScenarioAgents" is special: The SpawnPoint will only spawn scenario agents that are part of this selection.
The Ego Agent need not be included in the "ScenarioAgents" selection; it will always be spawned.

Example
```xml
    <Entities>
        <ScenarioObject name="Ego">
            <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent"/>
        </ScenarioObject>
        <ScenarioObject name="ScenarioAgent1">
            <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent"/>
        </ScenarioObject>
        <ScenarioObject name="ScenarioAgent2">
            <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent"/>
        </ScenarioObject>
        <EntitySelection name="ScenarioAgents">
            <Members>
                <EntityRef entityRef="ScenarioAgent1" />
                <EntityRef entityRef="ScenarioAgent2" />
            </Members>
        </EntitySelection>
    </Entities>
```

Parameters defined in the VehicleCatalog can be assigned inside the CatalogReference element.
In this case the assigned parameter will overwrite the definition in the VehicleCatalog.
Otherwise the value defined in the VehicleCatalog is used.
The type of the parameter must match those in the VehicleCatalog.

Example
```xml
    <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent">
        <ParameterAssignments>
            <ParameterAssignment parameterRef="Length" value="4.0" />
        </ParameterAssignments>
    </CatalogReference>
```

Note: In OpenPASS we reference an AgentProfile instead of a VehicleModel. An AgentProfile may have a stochastic distribution of different VehicleModel with different parameters.
In this case some assigned parameters may only be used if a certain VehicleModel is drawn in the random selection.

\section scenario_storyboard Storyboard

The <Storyboard> tag contains the initial setup of the scenario as well as manipulator actions that affect the behaviour of the simulation during runtime if predefined conditions are met.

\subsection scenario_storyboard_init Init

The content of the <Init> tag is forwarded to the SpawnPoint to define if and where it should place the ego and scenario agents.
The position can either be defined as global coordinates (x and y) with the World tag or as lane coordinates (s and t) with the Lane tag.
The schema is as follows:

|tag				|parent			|attributes					|multiplicity|
|-------------------|---------------|---------------------------|------------|
|Actions			|Init			|-							|1		     |
|Private    		|Actions		|entityRef					|1 per entity|
|PrivateAction		|Private    	|-							|at least 1  |
|TeleportAction		|PrivateAction	|-							|1		     |
|Position			|TeleportAction	|-							|1		     |
|LanePosition		|Position		|roadId, laneId, s, offset	|0 or 1		 |
|WorldPosition		|Position		|x, y, h	                |0 or 1		 |
|Orientation		|LanePosition	|type, h					|0 or 1		 |
|LongitudinalAction |PrivateAction		|-						|1		     |
|SpeedAction		|LongitudinalAction	|-						|1			 |
|SpeedActionDynamics|SpeedAction	|rate, dynamicsShape		|1			 |
|SpeedActionTarget	|SpeedAction	|-							|1			 |
|AbsoluteTargetSpeed|SpeedActionTarget |value					|1			 |
|RoutingAction    	|PrivateAction  |-                          |0 or 1      |
|AssignRouteAction	|RoutingAction  |-                          |1           |
|Route      		|AssignRouteAction    |-                    |1           |
|Waypoint   		|Route          |-                          |at least 1  |
|Position   		|Waypoint       |-                          |1           |
|RoadPosition       |Position       |roadId, t                  |1           |
|VisibilityAction   |PrivateAction  |traffic					|1           |


All listed attributes are required.
The attributes have the following meaning:

|tag		            |attribute	|meaning																					 |
|-----------------------|-----------|--------------------------------------------------------------------------------------------|
|Private	            |entityRef  |name of the entity for which the initial values are described in the subtags				 |
|LanePosition           |roadId		|Id of the road in the Scenery																 |
|LanePosition           |laneId		|Id of the lane in the Scenery																 |
|LanePosition           |s			|start position on the lane (i.e. distance from the start of the road) of the reference point|
|LanePosition           |offset		|lateral distance of the reference point to the middle of the road (i.e. t coordinate)		|
|WorldPosition          |x		    |x coordinate of the reference point			                                            |
|WorldPosition          |y		    |y coordinate of the reference point			                                            |
|WorldPosition          |h		    |heading			                                                                        |
|Orientation            |type		|has to be "relative"																		|
|Orientation            |h			|heading angle in radian relative to the lane												|
|SpeedActionDynamics    |rate		|acceleration																			    |
|SpeedActionDynamics    |dynamicsShape|"linear" for constant acceleration, "step" for immediate transition				      	|
|SpeedActionDynamics    |dynamicsDimension|has to be "rate"                                                         	      	|
|AbsoluteTargetSpeed    |value		|velocity																			    	|
|RoadPosition           |roadId     |Id of the road in the Scenery																|
|RoadPosition           |t          |negative for driving in roadDirection (i.e on lanes with negative Id) else positive        |
|RoadPosition           |s          |ignored                                                                                    |
|VisibilityAction       |traffic    |Flag deciding if the scenario agent will be spawned (true = spawned, false = not spawned)  |

Although OpenSCENARIO also states other ways for defining a position, we currently only support position via the <LanePosition> or the <WorldPosition> tag.
Unlike OpenSCENARIO we also allow some of these values to be stochastic.
This is marked by adding a subtag  <Stochastics value="valuetype" stdDeviation="value" lowerBound="value" upperBound="value"/> to the <LanePosition> or <SpeedAction> tag.
The stochastics tag is intended to be used as NormalDistribution, but it is up to each module using it to define the actual usage.
The valuetype can either be s, offset (if inside LanePosition), velocity or rate (if inside SpeedAction).
The value defined as attribute of the LanePosition/SpeedActionDynamics/AbsoluteTargetSpeed tag is then taken as mean value.
The VisibilityAction is optional.
If VisibilityAction is not defined the agent will be spawned.

Example
```xml
	<Init>
		<Actions>
			<Private entityRef="Ego">
				<PrivateAction>
					<TeleportAction>
						<Position>
							<LanePosition roadId="1" s="20.0" laneId="-2" offset="0.0">
								<Orientation type="relative" h="0.2"/>
								<Stochastics value="s" stdDeviation="5.0" lowerBound="10.0" upperBound="30.0"/>
							</LanePosition>
						</Position>
					</TeleportAction>
				</PrivateAction>
				<PrivateAction>
					<LongitudinalAction>
						<SpeedAction>
							<SpeedActionDynamics rate="0.0" dynamicsShape="linear" dynamicsDimension="rate"/>
							<SpeedActionTarget>
								<AbsoluteTargetSpeed value="10.0" />
							</SpeedActionTarget>
                            <Stochastics value="velocity" stdDeviation="2.0" lowerBound="5.0" upperBound="15.0"/>
						</SpeedAction>
					</LongitudinalAction>
				</PrivateAction>
                <PrivateAction>
                    <RoutingAction>
                        <AssignRouteAction>
                            <Route>
                                <Waypoint>
                                    <Position>
                                        <RoadPosition roadId="1" t="-1.0" s="0"/>
                                    </Position>
                                </Waypoint>
                                <Waypoint>
                                    <Position>
                                        <RoadPosition roadId="2" t="-1.0" s="0"/>
                                    </Position>
                                </Waypoint>
                            </Route>
                        </AssignRouteAction>
                    </RoutingAction>
                </PrivateAction>
			</Private>
			<Private entityRef="ScenarioAgent">
				<PrivateAction>
					<TeleportAction>
						<Position>
							<LanePosition roadId="1" s="50.0" laneId="-3" offset="0.0"/>
						</Position>
					</TeleportAction>
				</PrivateAction>
				<PrivateAction>
					<LongitudinalAction>
						<SpeedAction>
							<SpeedActionDynamics rate="0.0" dynamicsShape="linear" dynamicsDimension="rate"/>
							<SpeedActionTarget>
								<AbsoluteTargetSpeed value="10.0" />
							</SpeedActionTarget>
						</SpeedAction>
					</LongitudinalAction>
				</PrivateAction>
			</Private>
		</Actions>
	</Init>
```

\subsection scenario_storyboard_story Story

Inside the (optional) <Story> tag all conditional interventions into the simulation during runtime are defined.
When importing this tag, its content is translated into EventDetectors and Manipulators.
An EventDetector checks in every timestep if its condition is fullfilled.
In this case it writes an event into the EventNetwork, which then triggers the associated Manipulator.
The story consists of multiple acts, which itself can consist of multiple maneuvergroups.
The grouping of maneuvergroups into acts currently has no effect in the simulator.
A maneuvergroups must consist of exactly one <Actors> tag and one <Maneuver> tag.
The maximumExecutionCount attribute states the maximum number of times that each maneuver will by triggered.
If the maximumExecutionCount is -1 then the maneuver can triggered an unlimited number of times.
Multiple stories can be added.

Example
```xml
	<Story name="MyStory">
		<Act name="Act1">
			<ManeuverGroup name="MySequence" maximumExecutionCount="-1">
				<Actors>
					...
				</Actors>
				<Maneuver name="MyManeuver">
					...
				</Maneuver>
			</ManeuverGroup>
		</Act>
	</Story>
```

\subsubsection scenario_storyboard_story_actors Actors

The <Actors> tag defines the agents that are mainly affected by this sequence.
These agents can either be defined directly by stating their name via subtags <EntityRef entityRef="entityName"/> or as the agent that triggered the event via <Actors selectTriggeringEntities="true"/>

Example
```xml
<Actors selectTriggeringEntities="false">
    <EntityRef entityRef="Ego"/>
</Actors>
```
\subsubsection scenario_storyboard_story_maneuver Maneuver

The <Maneuver> tag defines the conditions for the EventDetector and the resulting action in the simulator.
It contains one or more events each consisting of one action (Note: OpenSCENARIO allows more than one action, but we support only a single action) and one or more start conditions.
The EventDetector Manipulator pair is currently being tied together through the event name.
Therefore unique event names are required.

Example
```xml
	<Maneuver name="MyManeuver">
		<Event name="MyEvent" priority="overwrite">
			<Action name="MyManipulator">
				...
			</Action>
			<StartTrigger>
				<ConditionGroup>
					<Condition name="Conditional">
						...
					</Condition>
				</ConditionGroup>
			</StartTrigger>
		</Event>
	</Maneuver>
```

\paragraph scenario_storyboard_story_maneuver_conditions Conditions

The conditions are internally represented as EventDetector (the ConditionalEventDetector).
If all conditions are met the EventDetector writes an event into the EventNetwork.
OpenPASS supports five different kinds of condition.
Conditions are either ByEntityConditions or ByValueConditions.

\paragraph scenario_storyboard_story_maneuver_conditions_byEntity ByEntityConditions

All ByEntityConditions require triggering agents.
Ego and Scenario agents can be specified as triggering agents in the TriggeringEntities section of the ByEntity-Condition.
If no agents are specified in the TriggeringEntities section, all agents will be used as triggering agents.
OpenScenario specifies that a rule be applied to the TriggeringEntities element; currently, only "any" is supported (indicating that any of the listed entities may satisfy the condition independent of the others).

**RelativeLanePosition** 

This Condition evaluates specifically if an entity is within a relative lane position with regard to the specified object

Uses an object, lane-delta (dLane), and s-delta (ds) for evaluation

To evaluate this Condition, the s-delta is applied to the s-coordinate and the lane-delta is applied to the lane of the referenceObject (e.g referenceObject.s + ds),
 then the tolerance is applied to the s-coordinate both forward and backward to form an area within which triggeringEntities would satisfy this condition.

Example
```xml
<Condition name="Conditional">
    <ByEntityCondition>
        <TriggeringEntities triggeringEntitiesRule="any">
            <EntityRef entityRef="Agent"/>
        </TriggeringEntities>
        <EntityCondition>
            <ReachPositionCondition tolerance="3.0">
                <Position>
                    <RelativeLanePosition object="referenceEntity" dLane="-1" ds="-50.0"/>
                </Position>
            </ReachPositionCondition>
        </EntityCondition>
    </ByEntityCondition>
</Condition>
```

**RoadPosition**

This Condition evaluates specifically if a TriggeringEntity is within range of a particular s-coordinate on a specifed road.

Uses the roadId and the s-Coordinate for evaluation.

The EventDetector verifies if a TriggeringEntity is on the specified road and if the agent's s-coordinate is within the tolerance range.

The tolerance is applied both forward and backward. 

Example
```xml
<Condition name="Conditional">
    <ByEntityCondition>
        <TriggeringEntities triggeringEntitiesRule="any">
            <EntityRef entityRef="Agent"/>
        </TriggeringEntities>
        <EntityCondition>
            <ReachPositionCondition tolerance="3.0">
                <Position>
                    <RoadPosition roadId="road1" s="50.0" t="0.0"/>
                </Position>
            </ReachPositionCondition>
        </EntityCondition>
    </ByEntityCondition>
</Condition>
```

**RelativeSpeed**

This condition evaluates if the relative velocity of a TriggeringEntity with regard to a specified referenceEntity is less than, equal to, or greater than, a specified value.
  
When calculating the relative velocity, we use referenceEntity as the "static" point.
  
Below, if the TriggeringEntity has a velocity of 30.0 and the referenceEntity has a velocity of 10.0, the calculated relative velocity is 30.0 - 10.0 = 20.0; therefore, the condition is satisfied. If the velocities are reversed, the calculated relative velocity is 10.0 - 30.0 = -20.0, leaving the condition unsatisfied.

Example
```xml
<Condition name="Conditional">
    <ByEntityCondition>
        <TriggeringEntities triggeringEntitiesRule="any">
            <EntityRef entityRef="Agent"/>
        </TriggeringEntities>
        <EntityCondition>
            <RelativeSpeedCondition entityRef="referenceEntity" value="10.0" rule="greaterThan"/>
        </EntityCondition>
    </ByEntityCondition>
</Condition>
```

**TimeToCollision**

The TimeToCollisionCondition evaluates the Time To Collision (TTC) between the specified Triggering Entities and the specified reference Entity.
When the comparison of the calculated TTC to the specified TTC value using the provided rule is true, this Condition is satisfied.

The TTC is determined by projecting the movement of the agents in timesteps of 0.1s and taking the first timestep, at which the bounding boxes intersect.

Example
```xml
<Condition name="Conditional">
	  <ByEntityCondition>
        <TriggeringEntities triggeringEntitiesRule="any">
            <EntityRef entityRef="Agent"/>
		</TriggeringEntities>
		<EntityCondition>
			<TimeToCollisionCondition value="2.0" rule="lessThan">
				<TimeToCollisionConditionTarget>
					<EntityRef entityRef="referenceAgent"/>
				</TimeToCollisionConditionTarget>
			 </TimeToCollisionCondition>
		</EntityCondition>
	  </ByEntityCondition>
</Condition>
```

**TimeHeadway**

The TimeHeadwayCondition evaluates the TimeHeadway (THW) between the specified Triggering Entities and the specified reference Entity.
When the comparison of the calculated THW to the specified THW value using the provided rule is true, this Condition is satisfied.

The TTC is calculated by dividing the s distance of the agents by the velocity of the triggering agent.
If the freespace attribute is set to true the net distance of the bounding boxes is considered, otherwise the distance of the reference points.
The alongRoute attribute has to be true (i.e. distance is calculated by following the road = s coordinate distance)

Example
```xml
<Condition name="Conditional">
    <ByEntityCondition>
        <TriggeringEntities triggeringEntitiesRule="any">
            <EntityRef entityRef="Agent"/>
		</TriggeringEntities>
		<EntityCondition>
			<TimeHeadwayCondition value="2.0" rule="lessThan" entityRef="referenceAgent" freespace="true" alongRoute="true"/>
		</EntityCondition>
    </ByEntityCondition>
</Condition>
```

\paragraph scenario_storyboard_story_maneuver_conditions_byValue ByValueConditions

All ByValueConditions trigger based on a specified value and are unrelated to any specific agent. 

**SimulationTime**
Triggers at a specified time value, configured by the detector parameters in the [Scenario File](\ref io_input_scenario).
Time is specified in seconds.
The rule is required and only "greaterThan" is accepted as valid.

Example
```xml
<Condition name="Conditional">
	<ByValueCondition>
		<SimulationTimeCondition value="5.0" rule="greaterThan" />
	</ByValueCondition>
</Condition>
```

\paragraph scenario_storyboard_story_maneuver_actions Actions

The actions define what happens if all conditions of the maneuver are fullfilled.
These act on the actors defined for this maneuver.
Depending on the type of the action a different Manipulator is created, that becomes active if it the EventNetwork contains an event of the associated EventDetector.

**LaneChange**

The LaneChangeManipulator writes an event to the EventNetwork that is read by the OpenScenarioActions module and converted into a trajectory that performs a lange change.
The target lane be given either absolute or relative to a specific entity. The trajectory can either have a fixed length (in s) or a fixed time.
Currently, "sinusoidal" is the only type supported.

Example with absolute target and fixed length
```xml
<Action name="LaneChange">
    <PrivateAction>
        <LateralAction>
            <LaneChangeAction>
                <LaneChangeActionDynamics value="100.0" dynamicsShape="sinusoidal" dynamicsDimension="distance"/>
                <LaneChangeTarget>
                    <AbsoluteTargetLane value="-1"/>
                </LaneChangeTarget>
            </LaneChangeAction>
        </LateralAction>
    </PrivateAction>
</Action>
```

Example with relative target and fixed time
```xml
<Action name="LaneChange">
    <PrivateAction>
        <LateralAction>
            <LaneChangeAction>
                <LaneChangeActionDynamics value="2.0" dynamicsShape="sinusoidal" dynamicsDimension="time"/>
                <LaneChangeTarget>
                    <RelativeTargetLane entityRef="Ego" value="0"/>
                </LaneChangeTarget>
            </LaneChangeAction>
        </LateralAction>
    </PrivateAction>
</Action>
```

**Follow Trajectory**

The TrajectoryManipulator also an event to the EventNetwork which tells the Driver that he should a given trajectory.
The trajectory can be defined either directly in the story or in a separate TrajectoryCatalog.
For the points (vertices) of the trajectory we support only World position (and ignore z, pitch and roll).

Example
```xml
<Action name="FollowTrajectory">
    <PrivateAction>
        <RoutingAction>
            <FollowTrajectoryAction>
                <Trajectory name="TrajectoryA" closed="false" domain="time">
                    <Vertex reference="0.0">
                        <Position>
                            <World x="1.0" y="2.0" z="0.0" h="0.0" p="0.0" r="0.0" />
                        </Position>
                        <Shape>
                            <Polyline/>
                        </Shape>
                    </Vertex>
                    <Vertex reference="1.0">
                        <Position>
                            <World x="2.0" y="3.0" z="0.0" h="0.1" p="0.0" r="0.0" />
                        </Position>
                        <Shape>
                            <Polyline/>
                        </Shape>
                    </Vertex>
                </Trajectory>
            </FollowTrajectoryAction>
        </RoutingAction>
    </PrivateAction>
</Action>
```

Example using TrajectoryCatalog
```xml
<Action name="FollowTrajectory">
    <Private>
        <Routing>
            <FollowTrajectory>
                <CatalogReference catalogName="TrajectoryCatalog.xosc" entryName="TrajectoryA">
                <Longitudinal>
                    <None/>
                </Longitudinal>
                <Lateral purpose="position"/>
            </FollowTrajectory>
        </Routing>
    </Private>
</Action>
```

**Acquire Position**

The `AcquirePositionManipulator` adds an event to the `EventNetwork`, which tells the driver to acquire a given 
`openScenario::Position`. Currently only `openScenario::WorldPosition` and `openScenario::RelativeObjectPosition` are
supported. **This is only supported in the FMU_Wrapper component for OSMP messages**

Example (WorldPosition)
```xml
<Action name="AcquirePosition">
    <PrivateAction>
        <RoutingAction>
            <AcquirePositionAction>
                <Position>
                    <WorldPosition x="1.0" y="2.0" z="0.0" h="3.1415" p="0.0" r="0.0" />
                </Position>
            </AcquirePositionAction>
        </RoutingAction>
    </PrivateAction>
</Action>
```

Example (WorldPosition)
```xml
<OpenSCENARIO>
    <Entities>
    ...
        <ScenarioObject name="S1">
        ...
        </ScenarioObject>
    </Entities>
...
  <Storyboard>
    ...
    <Story name="AcquirePositionStory">
      <Act name="Act1">
        <ManeuverGroup maximumExecutionCount="1" name="AcquirePositionManeuverGroup">
          ...
          <Maneuver name="AcquirePositionManeuver">
            <Event name="AcquirePositionEvent" priority="overwrite">
              <Action name="AcquirePosition">
                <PrivateAction>
                  <RoutingAction>
                    <AcquirePositionAction>
                      <Position>
                        <RelativeObjectPosition dx="1.0" dy="1.0" entityRef="S1"/>
                      </Position>
                    </AcquirePositionAction>
                  </RoutingAction>
                </PrivateAction>
              </Action>
            </Event>
          </Maneuver>
        </ManeuverGroup>
      </Act>
    </Story>
  </Storyboard>
</OpenSCENARIO>
```

**Remove Agent**

The RemoveAgentsManipulator simply removes the specifed agent from the simulation run.

Example
```xml
<Action name="RemoveAgent">
	<GlobalAction>
		<EntityAction entityRef="">
			<DeleteEntityAction/>
		</EntityAction>
	</GlobalAction>
</Action>
```

**UserDefinedAction: Component State Change**

The ComponentStateChangeManipulator influences the maximum reachable state of an agent component that is handled by the ComponentController, i.e. it can either activate or deactivate a component.

Example
```xml
<Action name="ComponentStateChange">
	<UserDefinedAction>
		<CustomCommandAction>SetComponentState Dynamics_TrajectoryFollower Acting</CustomCommandAction>
	</UserDefinedAction>
</Action>
```

**UserDefinedAction: Custom Parameters**

The `CustomParametersManipulator` adds an event to the `EventNetwork`, which is relayed as list of strings (`CustomParametersSignal`).
Currently, arguments are seperated by a SINGLE WHITESPACE, so parameters which contain a space are split.
**FMU_Wrapper component for OSMP messages only**: The parameters are transformed into indiviudal custom actions of the TrafficCommand.

Example
```xml
<Action name="CustomParameters">
    <UserDefinedAction>
        <CustomCommandAction>SetCustomParameters ARG1 ARG2 ARG3</CustomCommandAction>
    </UserDefinedAction>
</Action>
```

**SpeedAction**

The SpeedAction adjusts the velocity of an agent based on parameters of the SpeedAction.
Both variants RelativeTargetSpeed and AbsoluteTargetSpeed are supported.
The SpeedActionDynamics attribute "dynamicsShape" currently only supports "linear" & "step".
The SpeedActionDynamics attribute "dynamicsDimension" currently only supports "rate".
The RelativeTargetSpeed attribute "continuous" is ignored.

Note: Values defined in the SpeedAction might not reflect actual values used by the simulator due to physical boundaries of the simulator.

Example AbsoulteTargetSpeed
```xml
<Action name="Adjust speed">
    <PrivateAction>
        <LongitudinalAction>
            <SpeedAction>
                <SpeedActionDynamics dynamicsShape="" value="" dynamicsDimension=""/>
                <SpeedActionTarget>
                    <AbsoluteTargetSpeed value="10.0"/>
                <SpeedActionTarget>
            </SpeedAction>
        </LongitudinalAction>
    </PrivateAction>
</Action>
```

Example RelativeTargetSpeed
```xml
<Action name="Adjust speed">
    <PrivateAction>
        <LongitudinalAction>
            <SpeedAction>
                <SpeedActionDynamics dynamicsShape="" value="" dynamicsDimension=""/>
                <SpeedActionTarget>
                    <RelativeTargetSpeed entityRef="refAgent" value="10.0" speedTargetValueType="delta" continuous="false" />
                </SpeedActionTarget>
            </SpeedAction>
        </LongitudinalAction>
    </PrivateAction>
</Action>
```

\subsection scenario_storyboard_endconditions EndConditions

Here the end conditions for the simulation are defined.
One SimulationTime condition is required, which is the time in seconds, and the rule has to be "greaterThan".
Any other or additional conditions are not supported.

Example:

```xml
<StopTrigger>
    <ConditionGroup>
        <Condition name="EndTime" delay="0" conditionEdge="rising">
            <ByValueCondition>
                <SimulationTimeCondition value="30.0" rule="greaterThan"/>
            </ByValueCondition>
        </Condition>
    </ConditionGroup>
</StopTrigger>
```
