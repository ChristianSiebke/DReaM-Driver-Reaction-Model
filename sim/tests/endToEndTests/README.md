# pyOpenPASS

## openPASS EndToEnd Test Framework

This tool acts as configurable executor for complete sets of configs for the openPASS Slave.

- Basic tests: Check executablitity of configs + Determinism test (*1 x n* vs *n x 1* tests).
- Specialized tests: Execute config and check for specific results in the output of the simulator.

## General Usage

Command:
```bash
main.py 
  -s SLAVE                              # path to slave, e.g. /openPASS/bin 
  -m MUTUAL_RESOURCES_PATH              # path to mutual config files for all runs, e.g. /examples/common  
  -r RESOURCES_PATH                     # path from where configs are retrieved (override common files if necessary)
  -c PYOPENPASS_CONFIG.JSON             # JSON config for pyOpenPASS
  --scope SCOPE_IN_PYOPENPASS_CONFIG    # A user defined scope in the config (such as fast, nightly, ...)
  -d                                    # Debug some pandas dataframes as csv to /tmp or C:\temp
```

Basic test config:
```json
{
    "scopes":
    {
        "THE_SCOPE":
        {
            "baseConfig":
            {
                "duration": 60,
                "randomSeed": 0,
                "invocations": 100
            },
            "configurations":
            [
              "CONFIG_UNDER_TEST1",
              "CONFIG_UNDER_TEST2"
            ]
        }
    }
}
```

Note that randomSeed is an optional value. 
If set, it will override the value given with the configurtation.

Parameterized test config:
```json
{
    "scopes":
    {
        "THE_SCOPE":
        {
            "baseConfig":
            {
                "duration": 60,
                "randomSeed": 0,
                "invocations": 100
            },
            "parameterization":
            {
              "file": "someConfigFile.xml",
              "xpath": "//some/value",
              "values": ["value1", "value2", "value3"]
            },
            "configurations":
            [
              "CONFIG_UNDER_TEST1",
              "CONFIG_UNDER_TEST2"
            ]
        }
    }
}
```

If parametization specified, the generic tests are executed for each config and each parameter value.
The value is injected into a single configuration file, specified via its filename and an valid xpath.
As values might be contain paths to resources, the variable `${configFolder}` can be used as dynamic placeholder.

**Example:**  
With the core being at `/OpenPASS/bin` the scope `FMU_Test` shall be executed using two different FMUs:  
`"values": ["${configFolder}/resources/FMUv1.fmu","${configFolder}/resources/FMUv2.fmu" ]`

For each parameter, a different config folder `configs/FMU_Test_#` is created, where `#` is the parameters index.
Before execution, the value of the first parameter `${configFolder}/resources/FMUv1.fmu` is replaced with the absolute path `/OpenPASS/bin/configs/FMU_Test_0/resources/FMUv1`.

Specialized test config:
```json
{
    "scopes":
    {
        "THE_SCOPE":
        {
            "baseConfig":
            {
                "duration": 30,
                "randomSeed": 0,
                "invocations": 100
            },
              "specialized":
            {
                "description": "A CONCISE DESCRIPTION OF THE SCOPE",
                "query": "A VALID QUERY STRING (SEE BELOW)",
                "success_rate": 0.9
            },
            "configurations":
            [
              "CONFIG_UNDER_TEST1",
              "CONFIG_UNDER_TEST2"
            ]
        }
    }
}
```

Note that description is an optional value, which is currently not used by the framework, but helps to remember, what the test is about.
It is strongy advised to set it, as it might be used in future releases.

Syntax for specialized queries:
```
[aggregate]([column] | [filter]) [operator] [value]
```

- Aggregate: Everything pandas supports on dataframes, such as [pandas.DataFrame.count](https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.count.html?highlight=count#pandas.DataFrame.count), min, max, mean
- Column:    A column on which the aggregate should operate.
             Columns are given by the simulation outputs cyclic columns, such as PositionRoute. 
             Additionally AgentId is made available.
- Filter:    A filter based on [pandas.DataFrame.filter](https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.filter.html?highlight=filter#pandas.DataFrame.filter) syntax using the available columns
- Operator:  A comparison operator from the following list: ==, <=, >=, <, >, !=, ~= (approximate). 
             The approximate operator allows `1*e-6 x value` as maximum deviation from value   
- Value:     A number

Example:
```
count(AgentId | PositionRoute >= 800 and Lane != -3) == 0
```

## Using events in filter

In order to query for a specific event, use `#(EVENT)` within the filter syntax.
Example:
```
count(AgentId | PositionRoute >= 800 and #(Collision) == True) == 0
```

Each event is associated with a set of triggering entity ids, affected entitiy ids, and arbitrary key/value pairs (please refer to the openPASS documentation for details).
This information is transformed into a "per agent" scope.

### Base query
For brevity, `Collision` is used as a template for arbitrary events.

Event: `Collision`  
Query: `#(Collision)`

#### TriggeringEntity
All agents, flagged as triggering become IsTriggering = True 

#### AffectedEntity
All agents, flagged as triggering become IsAffected = True 

#### Key/Value pair basics
- Key: `CollisionWithAgent`  
  Query: `#(Collision):WithAgent`

Note, duplicates in the Key (e.g. Collision/CollisionWithAgent) are removed -> Collision/WithAgent

#### Example  
*No agent should collide with agent 0:*  
`count(AgentId | AgentId == 0 and #(Collision):WithAgent == 1) == 0`

### openSCENARIO Events

OpenScenario events are processed in the same manner as regular events.

Allows to query for occurrences of openSCENARIO events with a name specified within the following xpath: 
`OpenSCENARIO/Story/Act/Sequence/Maneuver/Event/@name`

#### Generic openSCENARIO Events 
```xml
<Story name="TheStory">
  <Act name="TheAct">
    <Sequence name="TheSequence" numberOfExecutions="1">
      ...
      <Maneuver name="TheManeuver">
        ...
        <!-- example name "ttc event"-->
        <Event name="ttc event" priority="overwrite"> 
        ...
          <StartConditions>
            <ConditionGroup>
              <Condition name="Conditional">
                <ByEntity>
                  ...
                  <EntityCondition>
                     <TimeToCollision>
                       ...
                     </TimeToCollision> 
                  </EntityCondition>
                </ByEntity>
              </Condition>
            </ConditionGroup>
          </StartConditions>
        </Event>
        ...
      </Maneuver>
    </Sequence>
  </Act>
</Story>
```

Example
```xml
<Event Time="0" Source="OpenSCENARIO" Name="BrakingCurveStory/Act1/StateChangeSequenceEgo/StateChangeManeuverScenarioAgent/StateChangeEventScenarioAgent">
    <TriggeringEntities/>
    <AffectedEntities>
        <Entity Id="1"/>
    </AffectedEntities>
    <Parameters/>
</Event>
```

Query: `count(AgentId | #(BrakingCurveStory/Act1/StateChangeSequenceEgo/StateChangeManeuverScenarioAgent/StateChangeEventScenarioAgent) == True ) > 0`


### Time Shift Columns

Syntax: Column-Shift => PositionRoute-1 means PositionRoute at one timestep earlier
Example: Counting Agents passing PositionRoute == 350 on LaneId == -1
Query: `count(AgentId | LaneId == -1 and PositionRoute-1 < 350 and PositionRoute >= 350 ) > 0`

If a column happens to have a name like `Name-N` where N is an integer, time shifting would be applied mistakenly.
Thus, it is possible to escape columns using surrounding single quotes (e.g. ``'Name-1'``).

# Test Report Generator

Generates a report for results from the pyOpenPASS EndToEndTest Framework tool.

## General Usage

Command:

```bash
report.py 
    -r RESOURCE1, RESOURCE2, ...          # One or more resource folders
    -c PYOPENPASS_CONFIG.JSON             # JSON config for pyOpenPASS
    --scope SCOPE1, SCOPE2, ...           # One or more scopes to be considered  
    --results SIMULATOR_RESULT_PATH       # Path with results from simulator (should be the same for all scopes)
    --output PATH_FOR_REPORT              # Path where the report should be written to
```
