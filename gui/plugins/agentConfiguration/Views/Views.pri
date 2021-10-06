################################################################################
# Copyright (c) 2019 Volkswagen Group of America
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

FORMS += \
    $$PWD/AgentConfigurationView.ui \
    $$PWD/AgentMapView.ui \
    $$PWD/VehicleProfilesView.ui \
    $$PWD/DriverProfilesView.ui \
    $$PWD/SystemProfileView.ui

HEADERS += \
    $$PWD/../../../common/DelegateComboBoxView.h \
    $$PWD/../../../common/DelegateDoubleView.h \
    $$PWD/../../../common/EditDataCommand.h \
    $$PWD/WidgetView.h \
    $$PWD/AgentConfigurationView.h \
    $$PWD/AgentMapView.h \
    $$PWD/VehicleProfilesView.h \
    $$PWD/DriverProfilesView.h \
    $$PWD/SystemProfileView.h

SOURCES +=\
    $$PWD/../../../common/DelegateComboBoxView.cpp \
    $$PWD/../../../common/DelegateDoubleView.cpp \
    $$PWD/WidgetView.cpp \
    $$PWD/AgentConfigurationView.cpp \
    $$PWD/AgentMapView.cpp \
    $$PWD/VehicleProfilesView.cpp \
    $$PWD/DriverProfilesView.cpp \
    $$PWD/SystemProfileView.cpp


