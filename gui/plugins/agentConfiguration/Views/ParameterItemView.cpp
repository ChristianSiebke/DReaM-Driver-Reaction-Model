/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "ParameterItemView.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QString>
#include <QtMath>

#include "ui_ParameterItemView.h"

ParameterItemView::ParameterItemView(QString key,
                                     QString type,
                                     QString value,
                                     QString unit,
                                     QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterItemView)
{
    ui->setupUi(this);

    setLocale(QLocale::C);

    // determine the visualisation depending on the given data type
    if (type == "Double")
    {
        QDoubleSpinBox *spinbox = new QDoubleSpinBox(this);
        spinbox->setMinimumSize(0, 20);
        spinbox->setMaximumSize(64, 20);
        spinbox->setDecimals(3);
        spinbox->setSingleStep(pow(10, adjustStepSize(value.toDouble())));
        spinbox->setValue(value.toDouble());
        ui->formLayout->setWidget(0, QFormLayout::FieldRole, spinbox);

        connect(spinbox, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                this, &ParameterItemView::valueChanged);
    }
    else if (type == "Float")
    {
        QDoubleSpinBox *spinbox = new QDoubleSpinBox(this);
        spinbox->setMinimumSize(0, 20);
        spinbox->setMaximumSize(64, 20);
        spinbox->setDecimals(3);
        spinbox->setSingleStep(pow(10, adjustStepSize(value.toFloat())));
        spinbox->setValue(value.toFloat());
        ui->formLayout->setWidget(0, QFormLayout::FieldRole, spinbox);

        connect(spinbox, static_cast<void (QDoubleSpinBox::*)(QString const &)>(&QDoubleSpinBox::valueChanged),
                this, &ParameterItemView::valueChanged);
    }

    else if (type == "Bool")
    {
        QCheckBox *checkbox = new QCheckBox(this);
        checkbox->setChecked(value == "true" ? true : false);
        ui->formLayout->setWidget(0, QFormLayout::FieldRole, checkbox);

        connect(checkbox, &QCheckBox::toggled, this, &ParameterItemView::toggled);
    }
    else if (type == "Int")
    {
        QSpinBox *spinbox = new QSpinBox(this);
        spinbox->setMinimumSize(0, 20);
        spinbox->setMaximumSize(64, 20);
        spinbox->setValue(value.toInt());
        ui->formLayout->setWidget(0, QFormLayout::FieldRole, spinbox);

        connect(spinbox, static_cast<void (QSpinBox::*)(QString const &)>(&QSpinBox::valueChanged),
                this, &ParameterItemView::valueChanged);
    }
    else if (type == "String")
    {
        QLineEdit *lineedit = new QLineEdit(this);
        lineedit->setMinimumSize(0, 20);
        lineedit->setText(value);
        ui->formLayout->setWidget(0, QFormLayout::FieldRole, lineedit);

        // NOTE: this is only temporal. So far only one driver model exists with a fixed string parameter
        lineedit->setReadOnly(true);

        connect(lineedit, &QLineEdit::textChanged, this, &ParameterItemView::valueChanged);
    }

    if (!unit.isEmpty())
        ui->Key->setText(QString("%1 [%2]:").arg(key).arg(unit));
    else
        ui->Key->setText(QString("%1:").arg(key));
}

void ParameterItemView::toggled(bool checked)
{
    Q_EMIT valueChanged(checked ? QString("true") : QString("false"));
}

int ParameterItemView::adjustStepSize(double value)
{
    // Determine the number of decimals (with 3 max) in the value
    int i = 0;
    value = (value - qFloor(value)) * 10;
    while (value != 0 && i > -3)
    {
        value = (value - qFloor(value)) * 10;
        i -= 1;
    }
    return i;
}

ParameterItemView::~ParameterItemView()
{
    delete ui;
}
