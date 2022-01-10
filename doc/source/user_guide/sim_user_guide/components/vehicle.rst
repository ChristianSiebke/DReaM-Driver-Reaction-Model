..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _components_vehiclecomponents:

VehicleComponents
-----------------

.. _components_aeb:

AEB
~~~

The Autonomous Emergency Braking system checks if a collision is likely to occur in the near future and, if necessary, brakes to avoid the collision.
In each timestep, the system evaluates all objects detected by a Sensor and calculates the time to collision (TTC) for this object based on the perceived movement of the object.
If, for any object, the TTC is lower than the threshold of the component, then the component gets activated. The system deactivates if the TTC is larger than 1,5 times the threshold of the component.

.. table::
   :class: tight-table

   ====================================== ====== ==== ===================================================================================
   Attribute                              Type   Unit Description
   ====================================== ====== ==== ===================================================================================
   CollisionDetectionLongitudinalBoundary Double m    Additional length added the vehicle boundary when checking for collision detection
   CollisionDetectionLateralBoundary      Double m    Additional width added the vehicle boundary when checking for collision detection
   TTC                                    Double s    Time to collision which is used to trigger AEB
   Acceleration                           Double m/s² Braking acceleration when activated
   ====================================== ====== ==== ===================================================================================

.. code-block:: xml

   <ProfileGroup Type="AEB">
       <Profile Type="AEB" Name="AEB1">
           <Double Key="CollisionDetectionLongitudinalBoundary" Value="4.0"/>
           <Double Key="CollisionDetectionLateralBoundary" Value="1.5"/>
           <Double Key="TTC" Value="2.0"/>
           <Double Key="Acceleration" Value="-2"/>
       </Profile>
       ...
   </ProfileGroup>

.. _components_trajectoryfollower:

DynamicsTrajectoryFollower
~~~~~~~~~~~~~~~~~~~~~~~~~~

This module forces agents to drive according to a specific trajectory. The trajectory is defined in the scenario. This module is disabled by default and is activated if a trajectory from openSCENARIO is triggered.
It is always important that the trajectories matches the current scenery file, otherwise the Agent could be placed outside of valid lanes. If the agent gets placed on a invalid position, it will be deleted.

All attributes are required.

.. table::
   :class: tight-table

   ===================== ==== =======================================================================================================================
   Attribute             Type Description
   ===================== ==== =======================================================================================================================
   AutomaticDeactivation Bool If true, the trajectory follower relinquishes control of the vehicle after the final instruction in the TrajectoryFile.
   EnforceTrajectory     Bool If true, the trajectory follower overrides external input related to the vehicle's travel.
   ===================== ==== =======================================================================================================================

.. code-block:: xml

   <ProfileGroup Type="DynamicsTrajectoryFollower">
       <Profile Name="BasicTrajectoryFollower">
           <Bool Key="AutomaticDeactivation" Value="true"/>
           <Bool Key="EnforceTrajectory" Value="true"/>
       </Profile>
   </ProfileGroup>

.. _components_geometric2d:

SensorGeometric2D
~~~~~~~~~~~~~~~~~

This sensor is selected, when a sensor is parameterized as ProfileGroup "Geometric2D".

.. table::
   :class: tight-table

   =============================== ====== ==== ==================================================================================================
   Parameter                       Type   Unit Description
   =============================== ====== ==== ==================================================================================================
   DetectionRange                  Double m    Detection range
   EnableVisualObstruction         Bool        Activates 2D sensor obstruction calculation
   FailureProbability              Double      Probability of an object detection failure
   Latency                         Double s    Sensor latency
   OpeningAngleH                   Double rad  Horizontal opening angle
   RequiredPercentageOfVisibleArea Double      Required percentage of an object within the sensor cone to trigger a detection
   =============================== ====== ==== ==================================================================================================

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Configurations/ADAS_AEB_PreventingCollisionWithObstacle/ProfilesCatalog.xml
   :language: xml
   :dedent: 2
   :start-at: <ProfileGroup Type="Geometric2D">
   :end-at: </ProfileGroup>

.. note:: 
   
   Sensors also need a mounting position, defined w.r.t. the coordinate system of the vehicle (center of rear axis).
   See also :ref:`profilescatalog_vehicleprofiles`.
