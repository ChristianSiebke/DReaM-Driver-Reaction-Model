/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

//-----------------------------------------------------------------------------
//! @file  WindowInterface.h
//! @ingroup windowPlugin
//! @brief The Window Interface (WI) contains the functions managing the Window
//! View of openPASS application.
//!
//! @details The Window Interface (WI) contains the functions managing the Window
//! View of openPASS application.
//! \n
//! This interface belongs to the \ref windowPlugin (WP).
//!
//-----------------------------------------------------------------------------

#ifndef WINDOWINTERFACE_H
#define WINDOWINTERFACE_H

#include "openPASS/ServiceInterface.h"
#include <QList>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QWidget>

static ServiceManagerInterface::ID const WindowInterfaceID =
        ServiceManagerInterfaceID("openPASS.Window.WindowInterface");

/**
 * @brief This class manages the registration and deregistration of Window Views
 * which include the menu bar with its buttons and menu buttons.
 */
class WindowInterface : public QObject,
        public ServiceInterface<WindowInterface, WindowInterfaceID>
{
    Q_OBJECT

public:
    using ID = QString;                             //!< The identifier of the stacked widget
    using Button = QPushButton;                     //!< The button in the side bar displaying the corresponding widget
    using ButtonTitle = QString;                    //!< The title of the widget related button
    using ButtonType = unsigned short;              //!< The type of the widget related button
    using ButtonOrder = unsigned short;             //!< The order of the widget related button in the side bar
    using Widget = QWidget;                         //!< The stacked widget
    using Action = QPushButton;                     //!< The button in the menu of the stacked widget
    using ActionTitle = QString;                    //!< The title of the button in the stacked widget's menu
    using ActionMenu = QMenu;                       //!< The drop down menu of the button in the stacked widget's menu
    using Menu = QList<WindowInterface::Action *>;  //!< The list of buttons in the menu of the stacked widget

public:
    explicit WindowInterface(QObject * const parent = nullptr)
        : QObject(parent), ServiceInterface(this) {}
    virtual ~WindowInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Registers the Window View with the identifier \a id, including its selecting
    //! button, and its corresponding widget and menu bar.
    //!
    //! @param[in]      id          The identifier for a Window View.
    //! @param[in]      button      The button for selecting the Window View.
    //! @param[in]      widget      The widget corresponding to the Window View.
    //! @param[in]      menu        The menu correponding to the Window View.
    //-----------------------------------------------------------------------------
    virtual void add(WindowInterface::ID const & id,
                     WindowInterface::Button * const button,
                     WindowInterface::Widget * const widget = nullptr,
                     WindowInterface::Menu const & menu = {}) = 0;

    //-----------------------------------------------------------------------------
    //! Deregisters a Window View with the identifier \a id.
    //!
    //! @param[in]      id          The identifier for a Window View.
    //! @return                     True if the Window View were deregistred successfully.
    //-----------------------------------------------------------------------------
    virtual bool remove(WindowInterface::ID const & id) = 0;

    //-----------------------------------------------------------------------------
    //! Verifies if the Window View with the identifier \a id is already registred.
    //!
    //! @param[in]      id          The identifier for a Window View.
    //! @return                     True if the Window View is already registred.
    //-----------------------------------------------------------------------------
    virtual bool exists(WindowInterface::ID const & id) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Creates button for a Window View selection with the title \a title, of type
    //! \a type and located on the side bar of openPASS main window at position
    //! \a order.
    //!
    //! @param[in]      title       The title for the new button.
    //! @param[in]      type        The type for the new button.
    //! @param[in]      order       The order of the new button in the tool side bar.
    //! @return                     The new button related to a Window View.
    //-----------------------------------------------------------------------------
    static inline WindowInterface::Button * createButton(WindowInterface::ButtonTitle const & title,
                                                         WindowInterface::ButtonType const & type = 0,
                                                         WindowInterface::ButtonOrder const & order = -1)
    {
        WindowInterface::Button * const button = new WindowInterface::Button(title);
        button->setProperty("type", type);
        button->setProperty("order", order);
        return button;
    }

public:
    //-----------------------------------------------------------------------------
    //! Creates button in the menu bar of the Window View \a object with the title
    //! \a title and associated to a callback function \a function.
    //!
    //! @param[in]      title       The title for the new button.
    //! @param[in]      object      The Window View where to create the button.
    //! @param[in]      function    The callback function of the new button.
    //! @return                     The new button of the menu bar of the Window View.
    //-----------------------------------------------------------------------------
    template <typename Object, typename Function>
    static inline WindowInterface::Action * createAction(WindowInterface::ActionTitle const & title,
                                                         Object object,
                                                         Function function)
    {
        WindowInterface::Action * const action = new WindowInterface::Action(title);
        QObject::connect(action, &WindowInterface::Action::clicked, object, function);
        return action;
    }

    //-----------------------------------------------------------------------------
    //! Creates a menu button in the menu bar with the title \a title and associated
    //! to the menu widget \a menu.
    //!
    //! @param[in]      title       The title for the new button.
    //! @param[in]      menu        The menu widget associated to the new button.
    //! @return                     The new menu button for the menu bar.
    //-----------------------------------------------------------------------------
    static inline WindowInterface::Action * createAction(WindowInterface::ActionTitle const & title,
                                                         WindowInterface::ActionMenu * const menu)
    {
        WindowInterface::Action * const action = new WindowInterface::Action(title);
        action->setMenu(menu);
        return action;
    }

    //-----------------------------------------------------------------------------
    //! Creates a separator in the menu bar of a Window View.
    //-----------------------------------------------------------------------------
    static inline WindowInterface::Action * createSeparator()
    {
        return nullptr;
    }
};

#endif // WINDOWINTERFACE_H
