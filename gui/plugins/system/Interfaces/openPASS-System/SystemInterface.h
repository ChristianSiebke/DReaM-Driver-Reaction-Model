/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  SystemInterface.h
//! @brief ...
//!
//-----------------------------------------------------------------------------

#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include <QIODevice>
#include <QObject>
#include <QString>

#include "openPASS-System/SystemComponentManagerInterface.h"
#include "openPASS-System/SystemMapInterface.h"
#include "openPASS/ServiceInterface.h"

static ServiceManagerInterface::ID const SystemInterfaceID =
    ServiceManagerInterfaceID("openPASS.System.SystemInterface");

class SystemInterface : public QObject,
                        public ServiceInterface<SystemInterface, SystemInterfaceID>
{
    Q_OBJECT

public:
    explicit SystemInterface(QObject *const parent = nullptr) :
        QObject(parent), ServiceInterface(this)
    {
    }
    virtual ~SystemInterface() = default;

Q_SIGNALS:
    void cleared() const;
    void loaded() const;
    void saved() const;

public:
    virtual bool clear() = 0;
    virtual bool load(QString const &filepath, bool const *const dynamicMode) = 0;
    virtual bool load(QIODevice *const device, bool const *const dynamicMode) = 0;
    virtual bool save(QString const &filepath, bool const *const dynamicMode) const = 0;
    virtual bool save(QIODevice *const device, bool const *const dynamicMode) const = 0;

public:
    virtual SystemComponentManagerInterface *getComponents() const = 0;
    virtual SystemMapInterface *getSystems() const = 0;

public:
    virtual SystemInterface *createSystemModel() = 0;
};

#endif // SYSTEMINTERFACE_H
