################################################################################
# Copyright (c) 2019-2020 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

TEMPLATE = subdirs

SUBDIRS = \
    World_OSI \
    opSimulation_IntegrationTests \
    Spawner_IntegrationTests

World_OSI.subdir                        = ../../src/core/opSimulation/modules/World_OSI

opSimulation_IntegrationTests.subdir   = opSimulation_IntegrationTests
opSimulation_IntegrationTests.depends  = World_OSI
Spawner_IntegrationTests.subdir   = Spawner_IntegrationTests
