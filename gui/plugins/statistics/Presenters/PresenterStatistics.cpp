/*********************************************************************
* Copyright (c) 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "Presenters/PresenterStatistics.h"

PresenterStatistics::PresenterStatistics(ModelStatistics *modelHistograms,
                                         ViewStatistics *viewHistograms,
                                         QObject *parent) :
    QObject(parent), modelStatistics(modelHistograms), viewStatistics(viewHistograms)
{
    viewHistograms->SetModelHistograms(modelHistograms->GetItemModelHistograms());
    modelHistograms->SetSelectionModelHistograms(viewHistograms->GetSelectionModelHistograms());

    connect(viewHistograms, &ViewStatistics::ResultPathChanged,
            modelHistograms, &ModelStatistics::LoadPathData);

    connect(modelHistograms, &ModelStatistics::Clear,
            viewHistograms, &ViewStatistics::OnClear);

    connect(modelHistograms, &ModelStatistics::HistogramAdded,
            viewHistograms, &ViewStatistics::PlotHistograms);

    connect(modelHistograms, &ModelStatistics::ShowMessage,
            viewHistograms, &ViewStatistics::ShowMessage);
}
