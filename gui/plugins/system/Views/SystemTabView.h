/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef SYSTEMTABVIEW_H
#define SYSTEMTABVIEW_H

#include "Views/WidgetView.h"
#include "openPASS-System/SystemComponentManagerInterface.h"
#include "openPASS-System/SystemMapInterface.h"

namespace Ui {
class SystemTabView;
}

class SystemTabView : public WidgetView
{
    Q_OBJECT

public:
    explicit SystemTabView(SystemMapInterface *const systems,
                           SystemComponentManagerInterface const *const components,
                           bool const *const dynamicMode,
                           QWidget *const parent = nullptr);
    virtual ~SystemTabView();

private Q_SLOTS:
    void on_systemTabs_tabBarDoubleClicked(int index);
    void changeTabTitle(SystemMapInterface::ID const &id);

public Q_SLOTS:
    void addTab(SystemItemInterface *const system);
    void removeTab(int index);
    void clear();

private:
    SystemMapInterface *const systems;
    SystemComponentManagerInterface const *const components;
    bool const *const dynamicMode;
    Ui::SystemTabView *ui;
};

#endif // SYSTEMTABVIEW_H
