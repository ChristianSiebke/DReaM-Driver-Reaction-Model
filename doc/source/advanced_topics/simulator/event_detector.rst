..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _eventdetector:

EventDetector
=============

EventDetectors are executed at the beginning of each timestep.
A detector uses its access to the world and the framework to detect specific events.
After detection, the event is usually forwarded to the EventNetwork.

There are multiple EventDetectors available, all of them sharing a common interface.

.. note:: Each instance of an EventDetector can have its own cycle time.


.. _eventdetector_collisiondetector:

CollisionDetector
-----------------

The CollisionDetector checks whether a collision has occurred.
Moving and stationary objects are taken into account.
Objects are considered as collided, if their two-dimensional bounding boxes intersect.

.. note:: The calculation of the bounding box itself considers a potential inclination by the roll angle of an object (projection onto the xy-plane).

In case of a collision, a CollisionEvent containing the participating object Ids is created.
This type of event is picked up by the CollisionManipulator, which updates the state of the collided objects accordingly.

