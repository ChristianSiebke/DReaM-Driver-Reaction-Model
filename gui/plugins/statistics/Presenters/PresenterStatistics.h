/*********************************************************************
* Copyright (c) 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef PRESENTERHISTOGRAMS_H
#define PRESENTERHISTOGRAMS_H

#include <QObject>

#include "Models/ModelStatistics.h"
#include "Views/ViewStatistics.h"

class PresenterStatistics : public QObject
{
    Q_OBJECT

public:
    explicit PresenterStatistics(ModelStatistics *modelStatistics,
                                 ViewStatistics *viewStatistics,
                                 QObject *parent = nullptr);
    virtual ~PresenterStatistics() = default;

private:
    ModelStatistics *const modelStatistics;
    ViewStatistics *const viewStatistics;
};

#endif // PRESENTERHISTOGRAMS_H
