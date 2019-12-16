#----------------------------------------------------------------------------- 
# Copyright (c) 2019 Volkswagen Group of America.
# 
# This program and the accompanying materials are made 
# available under the terms of the Eclipse Public License 2.0 
# which is available at https://www.eclipse.org/legal/epl-2.0/ 
# 
# SPDX-License-Identifier: EPL-2.0 
#-----------------------------------------------------------------------------/ 

!contains(INCLUDEPATH, $$clean_path($$PWD/..)) {
    INCLUDEPATH += $$clean_path($$PWD/..)
}

HEADERS += \
    $$PWD/AgentConfigurationInterface.h \
    $$PWD/AgentMapInterface.h \
    $$PWD/AgentItemInterface.h \
    $$PWD/VehicleProfileMapInterface.h \
    $$PWD/VehicleProfileItemInterface.h \
    $$PWD/VehicleComponentMapInterface.h \
    $$PWD/VehicleComponentItemInterface.h \
    $$PWD/SensorMapInterface.h \
    $$PWD/SensorItemInterface.h \
    $$PWD/SensorProfileMapInterface.h \
    $$PWD/SensorProfileItemInterface.h \
    $$PWD/SensorProfileParameterMapInterface.h \
    $$PWD/SensorProfileParameterItemInterface.h \
    $$PWD/VehicleComponentProfileMapInterface.h \
    $$PWD/VehicleComponentProfileItemInterface.h \
    $$PWD/ParameterMapInterface.h \
    $$PWD/ParameterItemInterface.h \
    $$PWD/DriverProfileMapInterface.h \
    $$PWD/DriverProfileItemInterface.h
