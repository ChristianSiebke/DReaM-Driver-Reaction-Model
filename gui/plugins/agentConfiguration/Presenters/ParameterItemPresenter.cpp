/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "ParameterItemPresenter.h"

ParameterItemPresenter::ParameterItemPresenter(ParameterItemInterface *parameter,
                                               ParameterItemView *parameterView,
                                               QObject *parent) :
    QObject(parent), parameter(parameter), parameterView(parameterView)
{
    connect(parameterView, &ParameterItemView::valueChanged, this, &ParameterItemPresenter::setValue);
}

void ParameterItemPresenter::setValue(const QString &value)
{
    parameter->setValue(value);
}
