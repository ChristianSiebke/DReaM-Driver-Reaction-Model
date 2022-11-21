/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  WidgetView.h
//! @ingroup trafficSimulationPlugin
//! @brief A reimplementation of the QWidget class. It reimplements the QWidgets
//!        paint Event in order to enable proper processing of style sheets.
//-----------------------------------------------------------------------------

#ifndef WIDGETVIEW_H
#define WIDGETVIEW_H

#include <QWidget>

class QPaintEvent;

//-----------------------------------------------------------------------------
//! @brief A reimplementation of the QWidget class. It reimplements the QWidgets
//!        paint Event in order to enable proper processing of style sheets.
//-----------------------------------------------------------------------------
class WidgetView : public QWidget
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Constructor which sets a parent widget if passed as an argument.
    //!
    //! @param[in]      parent      A parent widget
    //-----------------------------------------------------------------------------
    explicit WidgetView(QWidget *const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~WidgetView() = default;

protected:
    //-----------------------------------------------------------------------------
    //! Reimplementation of the paint event for proper processing of style sheets.
    //-----------------------------------------------------------------------------
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // WIDGETVIEW_H
