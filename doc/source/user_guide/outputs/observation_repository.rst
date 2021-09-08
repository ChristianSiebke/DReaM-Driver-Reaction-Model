..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _observation_entityrepository:

Observation_EntityRepository
############################

This observer logs all entities generated for an experiment, split by run and/or persistence.
In the following, the parametrization and generated outputs of the ``Observation_EntityRepository`` observer are described.

.. _observation_entityrepository_parametrization:

Parametrization
===============

Following parameters are supported:

.. table::
   :class: tight-table

   ======================= ============= ===============================================================================================================
   Parameter               Type          Description
   ======================= ============= ===============================================================================================================
   FilenamePrefix          String        (Optional) Prefix for the output files.
                                         Defaults to ``Repository`` resulting e.g. in ``Repository_Run_00.csv``.
   WritePersistentEntities String        Defines how persistent, ie cross-run entities such as stationary objects, are logged
                                         
                                         Options:

                                         - **Consolidated** (default): Logged together with non persistent entities and hence duplicated for each run.
                                         - **Separate**: Written once into ``{FilenamePrefix}_Persistent.csv``
                                         - **Skip**: No output.
   ======================= ============= ===============================================================================================================

.. warning:: The visualization is searching for files with the default ``Repository`` prefix, so don't change it unless you have a reason to.

**Example**

.. literalinclude:: @OP_REL_SIM@/contrib/examples/DefaultConfigurations/slaveConfig.xml
   :language: xml
   :start-at: <Library>Observation_EntityRepository</Library>
   :end-at: </Parameters>

Output Files
============

.. _observation_entityrepository_run:

Repository_Run_###.csv
~~~~~~~~~~~~~~~~~~~~~~

This file contains information about entities, which exist only within the given run.
In other words, ``Repository_Run_000.csv`` will contain all moving objects, spawned within the first run (zero-based index).

**Example**

.. csv-table::
   :file: _static/Repository_Run_000.csv
   :header-rows: 1
   :delim: ;

.. note:: By convention, moving objects start with id 0.

If ``WritePersistentEntities`` is set to ``Consolidated``, each file will also contain information about stationary objects, described below.

.. _observation_entityrepository_persistent:

Repository_Persistent.csv
~~~~~~~~~~~~~~~~~~~~~~~~~

This file contains information about entities, which exist in every single run.
This includes mainly both objects (starting at 100000) and road elements (starting at 200000), defined by the :ref:`scenery`.

**Example**

.. table::

   ====== ================ ========= ======= ============ ============= =========== =======
   id     group            source    version name         secondary id  type        subtype
   ====== ================ ========= ======= ============ ============= =========== =======
   100000 StationaryObject OpenDRIVE 1.6     Barrier      barrier_01    obstacle
   100001 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_01  railing
   100002 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_01  railing
   100003 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_01  railing
   100004 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_01  railing
   100005 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_01  railing
   100006 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_02  railing
   100007 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_02  railing
   100008 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_02  railing
   100009 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_02  railing
   100010 StationaryObject OpenDRIVE 1.6     GuardRail    guardrail_02  railing
   200000 Others           OpenDRIVE 1.6     LaneRoadMark               Solid
   200001 Others           OpenDRIVE 1.6     LaneRoadMark               Solid
   200002 Others           OpenDRIVE 1.6     Lane         -3            Driving
   200003 Others           OpenDRIVE 1.6     LaneRoadMark               Broken
   200004 Others           OpenDRIVE 1.6     Lane         -2            Driving
   200005 Others           OpenDRIVE 1.6     LaneRoadMark               Broken
   200006 Others           OpenDRIVE 1.6     Lane         -1            Driving
   ====== ================ ========= ======= ============ ============= =========== =======

Information coming from the source ``openDRIVE`` are mapped in the following manner:

.. table::

   ========= ============
   Attribute Column
   ========= ============
   name      name
   id        secondary id
   type      type
   subtype   subtype
   ========= ============

.. note:: 

   Repeated OpenDRIVE objects are internally split into individual objects.
   E.g. a single guard rail object in openDRIVE, is split into individual openPASS objects having consecutive ids.
   In the example above, the openDRIVE object with (secondary) id ``guardrail_01`` is split into 5 individual openPASS objects with the closed id range from 100001 to 100005.