/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef SYSTEMCOMPONENTPARAMETERVALUENORMALDISTRIBUTIONVIEW_H
#define SYSTEMCOMPONENTPARAMETERVALUENORMALDISTRIBUTIONVIEW_H

#include <QDoubleSpinBox>
#include <QWidget>

class SystemComponentParameterItemInterface;

class SystemComponentParameterValueNormalDistributionView : public QWidget
{
    Q_OBJECT
public:
    explicit SystemComponentParameterValueNormalDistributionView(SystemComponentParameterItemInterface *const parameter,
                                                                 QWidget *parent = nullptr);
    virtual ~SystemComponentParameterValueNormalDistributionView() = default;

public Q_SLOTS:
    void updateInterface();
    void updateView();

protected:
    SystemComponentParameterItemInterface *const parameter;

private:
    QDoubleSpinBox *const meanView;
    QDoubleSpinBox *const sdView;
    QDoubleSpinBox *const minView;
    QDoubleSpinBox *const maxView;
};

#endif // SYSTEMCOMPONENTPARAMETERVALUENORMALDISTRIBUTIONVIEW_H
