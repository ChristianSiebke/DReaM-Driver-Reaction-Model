..
  *******************************************************************************
  Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
                2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************


Component View
==============

|Op| is divided into components, which can be roughly divided into **core components** and **model components**.
The most interesting core components are the individual **spawners**, which are responsible for populating the world in a realistic fashion.
Model components are again roughly divided into **drivers** and **vehicle components** and represent how an individual participant is composed.

.. note::
   
   There are several additional components acting in the background, making sure things work as they should.
   As a guideline, components parameterized through the **ProfilesCatalog** are scope of this guide, others not.

   
.. _simuserguide_components:

Components in Depth
-------------------

In the following, all available components are described.

.. toctree::
   :glob:
   :maxdepth: 3

   components/*
