/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef PARAMETERITEMVIEW_H
#define PARAMETERITEMVIEW_H

#include <QWidget>

namespace Ui {
class ParameterItemView;
}

class ParameterItemView : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterItemView(QString key,
                               QString type,
                               QString value,
                               QString unit,
                               QWidget *parent = 0);
    ~ParameterItemView();

Q_SIGNALS:
    void valueChanged(QString const &value);

private Q_SLOTS:
    void toggled(bool checked);

private:
    int adjustStepSize(double value);

private:
    Ui::ParameterItemView *ui;
};

#endif // PARAMETERITEMVIEW_H
