..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _scenery:

Scenery
=======

This file describes the road network and all static objects of the road for the simulation run.
It is structured according to the **ASAM OpenDRIVE 1.6** standard.

The file name can be freely chosen, but needs to be properly referenced by the :ref:`scenario` within the tag :ref:`scenario_roadnetwork`.

Restrictions
------------

If object definitions does not meet the openDRIVE or are not supported, the simulation is aborted.
On top |op| adds some additional requirements to the attributes of objects:

* length > 0
* width > 0
* radius == 0

Objects, which do no meet these requirements are ignored.

Full Example
------------

.. literalinclude:: @OP_REL_SIM@/contrib/examples/Common/SceneryConfiguration.xodr
   :language: xml
   :caption: SceneryConfiguration.xodr
   :linenos:
