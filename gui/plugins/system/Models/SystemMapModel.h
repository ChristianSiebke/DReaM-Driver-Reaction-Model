/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef SYSTEMMAPMODEL_H
#define SYSTEMMAPMODEL_H

#include "openPASS-System/SystemMapInterface.h"

class SystemMapModel : public SystemMapInterface
{
    Q_OBJECT

public:
    explicit SystemMapModel(QObject *const parent = nullptr);
    virtual ~SystemMapModel() = default;

public:
    virtual SystemMapInterface::Iterator begin() override;
    virtual SystemMapInterface::ConstIterator begin() const override;

public:
    virtual SystemMapInterface::Iterator end() override;
    virtual SystemMapInterface::ConstIterator end() const override;

public:
    virtual SystemMapInterface::ID getID(SystemMapInterface::Item *const item) const override;
    virtual SystemMapInterface::Item *getItem(SystemMapInterface::ID const &id) const override;

public:
    virtual SystemMapInterface::ID generateID() override;
    virtual SystemItemInterface::Title generateTitle() override;

public:
    virtual bool add(SystemMapInterface::ID const &id) override;
    virtual bool add(SystemMapInterface::ID const &id,
                     SystemMapInterface::Item *const system) override;

public:
    virtual bool remove(SystemMapInterface::ID const &id) override;

public:
    virtual bool contains(SystemItemInterface::ID const &id) override;
    virtual bool contains(SystemItemInterface::Title const &title) override;

public:
    virtual int count() const override;

public:
    virtual bool clear() override;

public:
    virtual QList<Item *> values() const override;

protected:
    SystemMapInterface::Map systems;
};

#endif // SYSTEMMAPMODEL_H
