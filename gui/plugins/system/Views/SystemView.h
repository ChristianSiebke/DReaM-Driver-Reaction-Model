/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef SYSTEMVIEW_H
#define SYSTEMVIEW_H

#include "Views/WidgetView.h"
#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-System/SystemInterface.h"
#include "openPASS-Window/WindowInterface.h"

namespace Ui {
class SystemView;
}

class SystemView : public WidgetView
{
    Q_OBJECT

public:
    explicit SystemView(WindowInterface *const window,
                        SystemInterface *const system,
                        ProjectInterface *const project,
                        QWidget *const parent = nullptr);
    virtual ~SystemView();

protected:
    static WindowInterface::ID const ViewID;

Q_SIGNALS:
    void dynamicModeActicated(bool dynamicMode) const;

protected Q_SLOTS:
    void actionNewSystem_clicked();
    void actionClear_clicked();
    void actionLoadSystem_clicked();
    void actionSaveSystem_clicked();
    void actionSaveScreenshot_clicked();
    void activateDynamicMode(bool checked);

public:
    bool isDynamicMode() const;

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    WindowInterface *const window;
    QString directory;
    SystemInterface *const system;
    bool *const dynamicMode;

protected:
    Ui::SystemView *const ui;
};

#endif // SYSTEMVIEW_H
