/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  StatusBarView.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines a status bar.
//!
//!        It comprises a ui object (defined in StatusBarView.ui) which includes
//!        a Text Browser (QTextBrowser) for showing text and a button (QPushButton)
//!        for showing and hiding the status bar. The status bar can be used to show,
//!        for instance, information relevant for the simulation.
//-----------------------------------------------------------------------------

#ifndef STATUSBARVIEW_H
#define STATUSBARVIEW_H

#include <QWidget>

namespace Ui {
class StatusBarView;
}

//-----------------------------------------------------------------------------
//! @brief This class defines a status bar.
//!
//!        It comprises a ui object (defined in StatusBarView.ui) which includes
//!        a Text Browser (QTextBrowser) for showing text and a button (QPushButton)
//!        for showing and hiding the status bar. The status bar can be used to show,
//!        for instance, information relevant for the simulation.
//-----------------------------------------------------------------------------
class StatusBarView : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBarView(QWidget *parent = 0);
    ~StatusBarView();

    enum class Color
    {
        Error = 0,
        Warning = 1,
        Note = 2,
        Message = 3
    };

private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! This slot is triggered when the hide/show button is clicked. Hides or shows
    //! the status bar.
    //-----------------------------------------------------------------------------
    void on_button_clicked();

public:
    //-----------------------------------------------------------------------------
    //! Method which displays the given text in the given color in the text browser.
    //!
    //! @param[in]      text        The text to be displayed
    //! @param[in]      color       The color of the text
    //-----------------------------------------------------------------------------
    void showText(QString text, Color color);

    //-----------------------------------------------------------------------------
    //! Reimplementation of show. Sets the visible status of the text browser and
    //! the checked status of the button to 'true'.
    //-----------------------------------------------------------------------------
    virtual void show();

    //-----------------------------------------------------------------------------
    //! Reimplementation of hide. Sets the visible status of the text browser and
    //! the checked status of the button to 'false'.
    //-----------------------------------------------------------------------------
    virtual void hide();

private:
    static QMap<Color, QColor> colorMap; //!< the map containing the colors depending on the message type.
    Ui::StatusBarView *ui;               //!< The ui object defining the status bar's appearance. It contains a QTextBrowser and a QPushButton object.
};

#endif // STATUSBARVIEW_H
