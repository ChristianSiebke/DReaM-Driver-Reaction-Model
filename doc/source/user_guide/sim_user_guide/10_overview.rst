..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH
  Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************


Overview
========

|Op| is a tool for executing a traffic based Monte-Carlo simulation and provides capabilities for the following two levels of variation:

#. **Experiment:** Topmost, an experiment defines the domains of possible inputs, which shall be compared, such as *"traffic at high volume"* with *"traffic at low volume"*.
   In this sense, a parameter variation is done at a very coarse level, resulting in two sets of inputs.
   Note that this level is covered by the GUI (see :ref:`pcm_simulation_guide`).

#. **Invocation:** When an input-set is available, |op| invokes a defined number of runs, resulting in a probabilistic sampling of the given state under parameterizable conditions.
   For example, in the aforementioned situation *"traffic at high volume"*, it would generate participants at *"lower speeds"* and *"smaller gaps"*.
   Thereby parameterizable means, that the user has the freedom to define probabilities defining *"lower speed"* and *"smaller gaps"*.
   In each invocation, |op| samples from these probabilities to generate a possible traffic situation under the given parameter variation.

