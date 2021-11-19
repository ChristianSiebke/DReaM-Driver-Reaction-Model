..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

OpenPASS Documentation
======================

The OpenPASS (Open Platform for Assessment of Safety Systems) tool is a developed framework for the simulation of interaction between traffic participants to evaluate and parametrize active safety systems. 
The simulation is based on a specific situation configuration and can contain several simulation runs, which differ due to random parameters.

The software suite of openPASS started as a set of stand-alone applications, which can be installed and configured individually.
Over time, especially the graphical user interface evolved to a single entry point, enabling the average user to use openPASS as a "monolithic" tool.

This guide contains information about installation, configuration and usage of all tools in the |Op| environment.

.. toctree::
   :caption: Installation Guides
   :glob:
   :maxdepth: 1

   installation_guide/*

The GUI lets the user configure the simulation and generate configuration files from all set parameters.
Based on these the simulation core calculates different simulation runs and compiles trace files for further processing.

.. toctree::
   :caption: User Guides
   :glob:
   :maxdepth: 1

   user_guide/*
   
.. toctree::
   :caption: Advanced topics
   :glob:
   :maxdepth: 1

   advanced_topics/*
   
.. include:: api.rst

.. toctree::
   :caption: Other Information
   :maxdepth: 1

   glossary.rst
   license.rst

Todolist
========

.. todolist::
