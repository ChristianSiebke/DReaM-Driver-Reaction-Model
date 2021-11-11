/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "SystemComponentParameterValueNormalDistributionView.h"

#include <QHBoxLayout>
#include <QLabel>

#include "openPASS-System/SystemComponentParameterItemInterface.h"

SystemComponentParameterValueNormalDistributionView::SystemComponentParameterValueNormalDistributionView(SystemComponentParameterItemInterface *const parameter,
                                                                                                         QWidget *parent) :
    QWidget(parent), parameter(parameter), meanView(new QDoubleSpinBox(this)), sdView(new QDoubleSpinBox(this)), minView(new QDoubleSpinBox(this)), maxView(new QDoubleSpinBox(this))
{
    meanView->setDecimals(3);
    sdView->setDecimals(3);
    minView->setDecimals(3);
    maxView->setDecimals(3);

    meanView->setFrame(false);
    sdView->setFrame(false);
    minView->setFrame(false);
    maxView->setFrame(false);

    meanView->setToolTip("mean");
    sdView->setToolTip("standard deviation");
    minView->setToolTip("minimum");
    maxView->setToolTip("maximum");

    // layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    layout->insertWidget(0, new QLabel("~N "));
    layout->insertWidget(1, meanView);

    layout->insertWidget(2, new QLabel(","));
    layout->insertWidget(3, sdView);

    layout->insertWidget(4, new QLabel(","));
    layout->insertWidget(5, minView);

    layout->insertWidget(6, new QLabel(","));
    layout->insertWidget(7, maxView);

    setLayout(layout);

    connect(parameter, &SystemComponentParameterItemInterface::modifiedValue,
            this, &SystemComponentParameterValueNormalDistributionView::updateView);

    connect(meanView, &QDoubleSpinBox::editingFinished,
            this, &SystemComponentParameterValueNormalDistributionView::updateInterface);

    connect(sdView, &QDoubleSpinBox::editingFinished,
            this, &SystemComponentParameterValueNormalDistributionView::updateInterface);

    connect(minView, &QDoubleSpinBox::editingFinished,
            this, &SystemComponentParameterValueNormalDistributionView::updateInterface);

    connect(maxView, &QDoubleSpinBox::editingFinished,
            this, &SystemComponentParameterValueNormalDistributionView::updateInterface);

    updateView();
}

void SystemComponentParameterValueNormalDistributionView::updateView()
{
    SystemComponentParameterItemInterface::Value value = parameter->getValue();

    meanView->setValue(value.section(",", 0, 0).toDouble());
    sdView->setValue(value.section(",", 1, 1).toDouble());
    minView->setValue(value.section(",", 2, 2).toDouble());
    maxView->setValue(value.section(",", 3, 3).toDouble());
}

void SystemComponentParameterValueNormalDistributionView::updateInterface()
{
    SystemComponentParameterItemInterface::Value value;

    value.append(QString::number(meanView->value()) + ",");
    value.append(QString::number(sdView->value()) + ",");
    value.append(QString::number(minView->value()) + ",");
    value.append(QString::number(maxView->value()));

    parameter->setValue(value);
}
