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
//! @file  ParameterMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of ParameterMapInterface.
//-----------------------------------------------------------------------------

#ifndef PARAMETERMAPMODEL_H
#define PARAMETERMAPMODEL_H

#include "openPASS-AgentConfiguration/ParameterMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of ParameterMapInterface.
//-----------------------------------------------------------------------------
class ParameterMapModel : public ParameterMapInterface
{
    Q_OBJECT

public:
    ParameterMapModel(QObject *parent = nullptr);
    virtual ~ParameterMapModel() override = default;

public:
    virtual ParameterMapInterface::Iterator begin() override;
    virtual ParameterMapInterface::ConstIterator constBegin() const override;

public:
    virtual ParameterMapInterface::Iterator end() override;
    virtual ParameterMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(ParameterMapInterface::ID const &id) override;
    virtual bool add(ParameterMapInterface::ID const &id, ParameterMapInterface::Item const *const item) override;
    virtual bool add(ParameterMapInterface::Item *const item) override;

public:
    virtual bool remove(ParameterMapInterface::ID const &id) override;
    virtual bool remove(ParameterMapInterface::Item *const item) override;

public:
    virtual ParameterMapInterface::Item *getItem(ParameterMapInterface::ID const &id) const override;

public:
    virtual void setID(ParameterMapInterface::Item *const item,
                       ParameterMapInterface::ID const &id) override;

    virtual ParameterMapInterface::ID getID(ParameterMapInterface::Item *const item) const override;

    virtual ParameterMapInterface::ID generateID() override;

public:
    virtual bool contains(ParameterMapInterface::ID const &id) const override;
    virtual int count() const override;

public:
    virtual void clear() override;

public:
    virtual QList<ID> keys() const override;
    virtual QList<Item *> values() const override;

private:
    Map parameters; //!< the map (container) for registering parameters
};

#endif // PARAMETERMAPMODEL_H
