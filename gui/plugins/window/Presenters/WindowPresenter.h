/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef WINDOWPRESENTER_H
#define WINDOWPRESENTER_H

#include <QObject>

#include "Models/WindowModel.h"

class WindowPresenter : public QObject
{
    Q_OBJECT

public:
    explicit WindowPresenter(WindowModel *const windowModel,
                             QObject *const parent = nullptr);
    virtual ~WindowPresenter() = default;

public:
    WindowModel::ViewList list() const;

Q_SIGNALS:
    void modified();

private:
    WindowModel *const windowModel;
};

#endif // WINDOWPRESENTER_H
