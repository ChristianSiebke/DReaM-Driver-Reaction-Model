..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _components_driver:

Driver
------

.. _components_agentfollowingdrivermodel:

AlgorithmAgentFollowingDriverModel
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This driver type adapts its velocity to an agent in front and holds a desired velocity if there's no front agent available (like adaptive cruise control). The lateral guidance always keeps the agent in the middle of the lane.

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/ProfilesCatalog.xml
   :language: xml
   :dedent: 4
   :start-at: <Profile Name="Regular">
   :end-at: </Profile>

.. table::
   :class: tight-table

   =========================== ========== ==== ====================================================================================================================== ============================
   Parameter                   Type       Unit Description                                                                                                            Defaults to
   =========================== ========== ==== ====================================================================================================================== ============================
   AlgorithmLateralModule      String          Behavior model for the steering wheel angle of the driver                                                              Required value
   AlgorithmLongitudinalModule String          Behavior model for the accelerator, brake pedal position, and the current gear of the driver                           Required value
   VelocityWish                Double     m/s  Desired speed                                                                                                          :abbr:`33.33 m/s (120 km/h)`
   Delta                       Double          Free acceleration exponent characterizing how the acceleration decreases with velocity (1: linear, infinity: constant) 4.0
   TGapWish                    Double     s    Desired time gap between ego and front agent                                                                           1.5 s
   MinDistance                 Double     m    Minimum distance between ego and front (used at slow speeds); Also called jam distance                                 2.0 m
   MaxAcceleration             Double     m/s² Maximum acceleration in satisfactory way, not vehicle possible acceleration                                            1.4 m/s²
   MaxDeceleration             Double     m/s² Desired deceleration                                                                                                   2.0 m/s²
   =========================== ========== ==== ====================================================================================================================== ============================

.. todo:: Check description of Delta
