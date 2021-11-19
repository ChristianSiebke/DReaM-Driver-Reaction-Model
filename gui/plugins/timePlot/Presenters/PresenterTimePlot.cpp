/********************************************************************************
 * Copyright (c) 2019-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Presenters/PresenterTimePlot.h"

PresenterTimePlot::PresenterTimePlot(ModelTimePlot *modelTimePlot,
                                     ViewTimePlot *viewTimePlot,
                                     QObject *parent) :
    QObject(parent), modelTimePlot(modelTimePlot), viewTimePlot(viewTimePlot)
{
    viewTimePlot->SetModelHighD(modelTimePlot->GetItemModelHighD());
    modelTimePlot->SetSelectionModelHighD(viewTimePlot->GetSelectionModelHighD());

    connect(viewTimePlot, &ViewTimePlot::ResultPathChanged,
            modelTimePlot, &ModelTimePlot::LoadPathData);

    connect(viewTimePlot, &ViewTimePlot::SendTableName,
            modelTimePlot, &ModelTimePlot::FindTableModel);

    connect(modelTimePlot, &ModelTimePlot::TableModelCreated,
            viewTimePlot, &ViewTimePlot::AddTableModel);

    connect(modelTimePlot, &ModelTimePlot::Clear,
            viewTimePlot, &ViewTimePlot::OnClear);

    connect(modelTimePlot, &ModelTimePlot::SendTableModel,
            viewTimePlot, &ViewTimePlot::PlotColumns);
}
