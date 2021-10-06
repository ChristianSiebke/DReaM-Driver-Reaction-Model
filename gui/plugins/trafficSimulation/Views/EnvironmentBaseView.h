/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef ENVIRONMENTBASEVIEW_H
#define ENVIRONMENTBASEVIEW_H

#include "EnvironmentView.h"
#include "WidgetView.h"

class EnvironmentBaseView : public WidgetView
{
public:
    EnvironmentBaseView(QWidget *parent = 0);
    ~EnvironmentBaseView() = default;

public Q_SLOTS:
    void updateSize();
    void adjustWidgets(QWidget *mainWindow);

public:
    EnvironmentView *getEnvironmentView() const;

public:
    EnvironmentView *environmentView;
};

#endif // ENVIRONMENTBASEVIEW_H
