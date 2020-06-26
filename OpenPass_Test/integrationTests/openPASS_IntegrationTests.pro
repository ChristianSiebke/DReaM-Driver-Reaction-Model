#/*******************************************************************************
#* Copyright (c) 2019, 2020 in-tech GmbH
#*
#* This program and the accompanying materials are made
#* available under the terms of the Eclipse Public License 2.0
#* which is available at https://www.eclipse.org/legal/epl-2.0/
#*
#* SPDX-License-Identifier: EPL-2.0
#*******************************************************************************/

TEMPLATE = subdirs

SUBDIRS = \
    World_OSI \
    OpenPassSlave_IntegrationTests \
    Spawner_IntegrationTests

World_OSI.subdir                        = ../../OpenPass_Source_Code/openPASS/CoreModules/World_OSI

OpenPassSlave_IntegrationTests.subdir   = OpenPassSlave_IntegrationTests
OpenPassSlave_IntegrationTests.depends  = World_OSI
Spawner_IntegrationTests.subdir   = Spawner_IntegrationTests
