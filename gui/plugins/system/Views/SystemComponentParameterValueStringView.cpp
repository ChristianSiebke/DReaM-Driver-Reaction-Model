/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Views/SystemComponentParameterValueStringView.h"

#include "openPASS-System/SystemComponentParameterItemInterface.h"

SystemComponentParameterValueStringView::SystemComponentParameterValueStringView(SystemComponentParameterItemInterface *const parameter,
                                                                                 QWidget *const parent) :
    QLineEdit(parent), parameter(parameter)
{
    setFrame(false);
    connect(parameter, &SystemComponentParameterItemInterface::modifiedValue,
            this, &SystemComponentParameterValueStringView::updateView);
    connect(this, &SystemComponentParameterValueStringView::editingFinished,
            this, &SystemComponentParameterValueStringView::updateInterface);
    updateView();
}

void SystemComponentParameterValueStringView::updateInterface()
{
    parameter->setValue(text());
}

void SystemComponentParameterValueStringView::updateView()
{
    int const position = cursorPosition();
    setText(parameter->getValue());
    setCursorPosition(position);
}
