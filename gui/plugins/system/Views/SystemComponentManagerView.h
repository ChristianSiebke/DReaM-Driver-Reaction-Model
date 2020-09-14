/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

#ifndef SYSTEMCOMPONENTMANAGERVIEW_H
#define SYSTEMCOMPONENTMANAGERVIEW_H

#include <QWidget>

#include "openPASS-System/SystemComponentManagerInterface.h"

namespace Ui {
class SystemComponentManagerView;
}

class SystemComponentManagerView : public QWidget
{
    Q_OBJECT

public:
    explicit SystemComponentManagerView(SystemComponentManagerInterface *const components,
                                        bool const *const dynamicMode,
                                        QWidget *const parent = nullptr);
    virtual ~SystemComponentManagerView();

public Q_SLOTS:
    virtual void show();
    virtual void hide();
    virtual void updateManagerView();

protected Q_SLOTS:
    void on_button_clicked();

private:
    bool isForbidden(ComponentItemInterface const *const item);

    int numberOfOutputs(ComponentItemInterface::Title const &title);
    int numberOfInputs(ComponentItemInterface::Title const &title);
    int numberOfParameters(const ComponentItemInterface::Title &title);

private:
    SystemComponentManagerInterface *const _components;
    bool const *const dynamicMode;

private:
    QList<ComponentItemInterface::Name> const dynamicComponents = {"Algorithm_AEB", "Sensor_OSI", "Dynamics_TrajectoryFollower"};

protected:
    Ui::SystemComponentManagerView *const ui;
};

#endif // SYSTEMCOMPONENTMANAGERVIEW_H
