\page scenario Scenario Configuration

\tableofcontents

\section scenario_overview Overview

The scenario configuration file describes all dynamic configuration of a simulation run, i.e. the position of an agent as well as conditional events that alter the behaviour of the simulation during the run.
It adheres to the OpenSCENARIO standard, although we support only a subset of the features of the standard.
The different parts of the scenario configuration are described in the following chapters:

\section scenario_catalogs Catalog References

The <Catalogs> tag defines references to various other files that describe sub features of OpenSCENARIO.
We currently support only three of these references: the [VehicleCatalog](\ref io_input_vehiclemodels), which defines the vehicle models, and the [PedestrianCatalog](\ref io_input_pedestrianmodels), which defines the pedestrian models.
For the FollowTrajectory action one can also use a TrajectoryCatalog.

Example
```xml
    <Catalogs>
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
    </Catalogs>
```

\section scenario_roadnetwork RoadNetwork

The <RoadNetwork> tag contains the reference to the OpenDrive [SceneryConfiguration](\ref io_input_scenery).

Example
```xml
    <RoadNetwork>
        <Logics filepath="SceneryConfiguration.xodr"/>
        <SceneGraph filepath=""/>
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
        <Object name="Ego">
            <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent"/>
        </Object>
        <Object name="ScenarioAgent1">
            <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent"/>
        </Object>
        <Object name="ScenarioAgent2">
            <CatalogReference catalogName="ProfilesCatalog.xml" entryName="MiddleClassCarAgent"/>
        </Object>
        <Selection name="ScenarioAgents">
            <Members>
                <ByEntity name="ScenarioAgent1" />
                <ByEntity name="ScenarioAgent2" />
            </Members>
        </Selection>
    </Entities>
```

\section scenario_storyboard Storyboard

The <Storyboard> tag contains the initial setup of the scenario as well as manipulator actions that affect the behaviour of the simulation during runtime if predefined conditions are met.

\subsection scenario_storyboard_init Init

The content of the <Init> tag is forwarded to the SpawnPoint to define where it should place the ego and scenario agents.
The schema is as follows:

|tag		|parent			|attributes					|multiplicity|
|-----------|---------------|---------------------------|------------|
|Actions	|Init			|-							|1		     |
|Private	|Actions		|object						|1 per entity|
|Position	|Private		|-							|1		     |
|Lane		|Position		|roadId, laneId, s, offset	|1			 |
|Orientation|Lane			|type, h					|0 or 1		 |
|Longitudinal|Action		|-							|1		     |
|Speed		|Longitudinal	|-							|1			 |
|Dynamics	|Speed			|rate, shape				|1			 |
|Target		|Speed			|-							|1			 |
|Absolute	|Target			|value						|1			 |

All listed attributes are required.
The attributes have the following meaning:

|tag		|attribute	|meaning																						|
|-----------|-----------|-----------------------------------------------------------------------------------------------|
|Private	|object		|name of the entity for which the initial values are described in the subtags					|
|Lane		|roadId		|Id of the road in the Scenery																	|
|Lane		|laneId		|Id of the lane in the Scenery																	|
|Lane		|s			|start position on the lane (i.e. distance from the start of the road) of the reference point	|
|Lane		|offset		|lateral distance of the reference point to the middle of the road (i.e. t coordinate)			|
|Orientation|type		|has to be "relative"																			|
|Orientation|h			|heading angle in radiant relative to the lane													|
|Dynamics	|rate		|acceleration																					|
|Dynamics	|shape		|unsupported (but required by OpenSCENARIO)														|
|Absolute	|Value		|velocity																						|

Although OpenSCENARIO also states other ways for defining a position, we currently only support position via the <Lane> tag.
Unlike OpenSCENARIO we also allow some of these values to be stochastic.
This is marked by adding a subtag  <Stochastics value="valuetype" stdDeviation="value" lowerBound="value" upperBound="value"/> to the <Lane> tag.
The valuetype can be one of s, offset, velocity or rate.
The value defined as attribute of the Lane/Dynamics/Absolute tag is then taken as mean value.

Example
```xml
	<Init>
		<Actions>
			<Private object="Ego">
				<Action>
					<Position>
						<Lane roadId="1" s="20.0" laneId="-2" offset="0.0">
							<Orientation type="relative" h="0.2"/>
							<Stochastics value="s" stdDeviation="5.0" lowerBound="10.0" upperBound="30.0"/>
						</Lane>
					</Position>
				</Action>
				<Action>
					<Longitudinal>
						<Speed>
							<Dynamics rate="0.0" shape="linear" />
							<Target>
								<Absolute value="10.0" />
							</Target>
						</Speed>
					</Longitudinal>
				</Action>
			</Private>
			<Private object="ScenarioAgent">
				<Action>
					<Position>
						<Lane roadId="1" s="50.0" laneId="-3" offset="0.0"/>
					</Position>
				</Action>
				<Action>
					<Longitudinal>
						<Speed>
							<Dynamics rate="0.0" shape="linear" />
							<Target>
								<Absolute value="10.0" />
							</Target>
						</Speed>
					</Longitudinal>
				</Action>
			</Private>
		</Actions>
	</Init>
```

\subsection scenario_storyboard_story Story

Inside the (optional) <Story> tag all conditional interventions into the simulation during runtime are defined.
When importing this tag its content is translated into EventDetectors and Manipulators.
An EventDetector checks in every timestep if its condition is fullfilled.
In this case it writes an event into the EventNetwork, which then triggers the associated Manipulator.
The story consists of multiple acts, which itself can consist of multiple sequences.
The grouping of sequences into acts currently has no effect in the simulator.
A sequence must consist of exactly one <Actors> tag and one <Maneuver> tag.
The numberOfExecutions attribute states the maximum number of times that the sequence will by triggered.
If the numberOfExecutions is -1 then the sequence can triggered an unlimited number of times.

Example
```xml
	<Story name="MyStory">
		<Act name="Act1">
			<Sequence name="MySequence" numberOfExecutions="-1">
				<Actors>
					...
				</Actors>
				<Maneuver name="MyManeuver">
					...
				</Maneuver>
			</Sequence>
		</Act>
	</Story>
```

\subsubsection scenario_storyboard_story_actors Actors

The <Actors> tag defines the agents that are mainly affected by this sequence.
These agents can either be defined directly by stating their name via <Entity name="entityName"/> or by a condition via <ByCondition actor="triggeringEntity"/>.
Currently "triggeringEntity" is the only supported condition.

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
			<StartConditions>
				<ConditionGroup>
					<Condition name="Conditional">
						...
					</Condition>
				</ConditionGroup>
			</StartConditions>
		</Event>
	</Maneuver>
```

\paragraph scenario_storyboard_story_maneuver_conditions Conditions

The conditions are internally represented as EventDetector (the ConditionalEventDetector).
If all conditions are met the EventDetector writes an event into the EventNetwork.
OpenPASS supports five different kinds of condition.
Conditions are either ByEntityConditions or ByValueConditions.

\paragraph scenario_storyboard_story_maneuver_conditions_byEntity ByEntity Conditions

All ByEntity Conditions require triggering agents.
Ego and Scenario agents can be specified as triggering agents in the TriggeringEntities section of the ByEntity-Condition.
If no agents are specified in the TriggeringEntities section, all agents will be used as triggering agents.
OpenScenario specifies that a rule be applied to the TriggeringEntities element; currently, only "any" is supported (indicating that any of the listed entities may satisfy the condition independent of the others).

**RelativeLane** 

This Condition evaluates specifically if an entity is within a relative lane position with regard to the specified object

Uses an object, lane-delta (dLane), and s-delta (ds) for evaluation

To evaluate this Condition, the s-delta is applied to the s-coordinate and the lane-delta is applied to the lane of the referenceObject (e.g referenceObject.s + ds),
 then the tolerance is applied to the s-coordinate both forward and backward to form an area within which triggeringEntities would satisfy this condition.

Example
```xml
<Condition name="Conditional">
        <ByEntity>
        	<TriggeringEntities rule="any">
        		<Entity name="Agent"/>
        	</TriggeringEntities>
        	<EntityCondition>
        		<ReachPosition tolerance="3.0">
        			<Position>
        				<RelativeLane object="referenceEntity" dLane="-1" ds="-50.0">
        			</Position>
        		</ReachPosition>
        	</EntityCondition>
        </ByEntity>
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
        <ByEntity>
        	<TriggeringEntities rule="any">
        		<Entity name="Agent"/>
        	</TriggeringEntities>
        	<EntityCondition>
        		<ReachPosition tolerance="3.0">
        			<Position>
        				<Road roadId="road1" s="50.0" t="0.0">
        			</Position>
        		</ReachPosition>
        	</EntityCondition>
        </ByEntity>
</Condition>
```

**RelativeSpeed**

This condition evaluates if the relative velocity of a TriggeringEntity with regard to a specified referenceEntity is less than, equal to, or greater than, a specified value.
  
When calculating the relative velocity, we use referenceEntity as the "static" point.
  
Below, if the TriggeringEntity has a velocity of 30.0 and the referenceEntity has a velocity of 10.0, the calculated relative velocity is 30.0 - 10.0 = 20.0; therefore, the condition is satisfied. If the velocities are reversed, the calculated relative velocity is 10.0 - 30.0 = -20.0, leaving the condition unsatisfied.

Example
```xml
<Condition name="Conditional">
        <ByEntity>
            <TriggeringEntities rule="any">
                <Entity name="Agent"/>
            </TriggeringEntities>
            <EntityCondition>
                <RelativeSpeed entity="referenceEntity" value="10.0" rule="greater_than"/>
            </EntityCondition>
        </ByEntity>
</Condition>
```

**TimeToCollision**

The TimeToCollision Condition evaluates the Time To Collision (TTC) between the specified Triggering Entities and the specified reference Entity.
When the comparison of the calculated TTC to the specified TTC value using the provided rule is true, this Condition is satisfied.

The TTC is determined by projecting the movement of the agents in timesteps of 0.1s and taking the first timestep, at which the bounding boxes intersect.

Example
```xml
<Condition name="Conditional">
	  <ByEntity>
		<TriggeringEntities rule="any">
			<Entity name="Agent"/>
		</TriggeringEntities>
		<EntityCondition>
			<TimeToCollision value="2.0" rule="less_than">
				  <Target>
					  <Entity name="referenceAgent"/>
				  </Target>
			  </TimeToCollision>
		</EntityCondition>
	  </ByEntity>
</Condition>
```

\paragraph scenario_storyboard_story_maneuver_conditions_byValue ByValue Conditions

All ByValue Conditions trigger based on a specified value and are unrelated to any specific agent. 

**SimulationTime**
Triggers at a specified time value, configured by the detector parameters in the [Scenario File](\ref io_input_scenario).
Time is specified in seconds.
The rule is required and only "greater_than" is accepted as valid.

Example
```xml
<Condition name="Conditional">
	<ByValue>
		<SimulationTime value="5.0" rule="greater_than" />
	</ByValue>
</Condition>
```

\paragraph scenario_storyboard_story_maneuver_actions Actions

The actions define what happens if all conditions of the maneuver are fullfilled.
These act on the actors defined for this maneuver.
Depending on the type of the action a different Manipulator is created, that becomes active if it the EventNetwork contains an event of the associated EventDetector.

**Lane Change**

The LaneChangeManipulator writes an event to the EventNetwork which tells the Driver that he should change one or more lanes to the left or right as specified by the <Target> tag.

Example
```xml
<Action name="LaneChange">
	<Private>
		<Lateral>
			<LaneChange>
				<Target>
					<Absolute value="-1"/>
				</Target>
			<LaneChange>
		</Lateral>
	</Private>
</Action>
```

**Follow Trajectory**

The TrajectoryManipulator also an event to the EventNetwork which tells the Driver that he should a given trajectory.
The trajectory can be defined either directly in the story or in a separate TrajectoryCatalog.
The Longitudinal and Lateral tag are currently ignored.
For the points (vertices) of the trajectory we support only World position (and ignore z, pitch and roll).

Example
```xml
<Action name="FollowTrajectory">
    <Private>
        <Routing>
            <FollowTrajectory>
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
                <Longitudinal>
                    <None/>
                </Longitudinal>
                <Lateral purpose="position"/>
            </FollowTrajectory>
        </Routing>
    </Private>
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

**Remove Agent**

The RemoveAgentsManipulator simply removes the specifed agent from the simulation run.

Example
```xml
<Action name="RemoveAgent">
	<Global>
		<Entity name="">
			<Delete/>
		</Entity>
	</Global>
</Action>
```

**Component State Change**

The ComponentStateChangeManipulator influences the maximum reachable state of an agent component that is handled by the ComponentController, i.e. it can either activate or deactivate a component.

Example
```xml
<Action name="ComponentStateChange">
	<UserDefined>
		<Command>SetComponentState Dynamics_TrajectoryFollower Acting</Command>
	</UserDefined>
</Action>
```

\subsection scenario_storyboard_endconditions EndConditions

Here the end conditions for the simulation are defined.
One SimulationTime condition is required, which is the time in seconds, and the rule has to be "greater_than".
Any other or additional conditions are not supported.

Example:

```xml
<EndConditions>
    <ConditionGroup>
        <Condition name="EndTime" rule="greater_than" edge="rising">
            <ByValue>
                <SimulationTime value="30.0" rule="greater_than" />
            </ByValue>
        </Condition>
    </ConditionGroup>
</EndConditions>
```
