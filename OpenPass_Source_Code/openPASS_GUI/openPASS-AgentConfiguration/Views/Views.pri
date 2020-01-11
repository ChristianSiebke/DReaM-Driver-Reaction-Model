# /*********************************************************************
# * Copyright (c) 2019 Volkswagen Group of America.
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

FORMS += \
    $$PWD/AgentConfigurationView.ui \
    $$PWD/AgentMapView.ui \
    $$PWD/VehicleProfilesView.ui \
    $$PWD/DriverProfilesView.ui \
    $$PWD/SystemProfileView.ui

HEADERS += \
    $$PWD/../../Global/DelegateComboBoxView.h \
    $$PWD/../../Global/DelegateDoubleView.h \
    $$PWD/../../Global/EditDataCommand.h \
    $$PWD/WidgetView.h \
    $$PWD/AgentConfigurationView.h \
    $$PWD/AgentMapView.h \
    $$PWD/VehicleProfilesView.h \
    $$PWD/DriverProfilesView.h \
    $$PWD/SystemProfileView.h

SOURCES +=\
    $$PWD/../../Global/DelegateComboBoxView.cpp \
    $$PWD/../../Global/DelegateDoubleView.cpp \
    $$PWD/WidgetView.cpp \
    $$PWD/AgentConfigurationView.cpp \
    $$PWD/AgentMapView.cpp \
    $$PWD/VehicleProfilesView.cpp \
    $$PWD/DriverProfilesView.cpp \
    $$PWD/SystemProfileView.cpp


