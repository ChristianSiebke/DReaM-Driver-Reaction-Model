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
//! @file  AgentMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of AgentMapInterface.
//-----------------------------------------------------------------------------

#ifndef AGENTMAPMODEL_H
#define AGENTMAPMODEL_H

#include "Models/AgentItemModel.h"
#include "openPASS-AgentConfiguration/AgentMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of AgentMapInterface.
//-----------------------------------------------------------------------------
class AgentMapModel : public AgentMapInterface
{
public:
    AgentMapModel(QObject *parent = nullptr);
    virtual ~AgentMapModel() override = default;

public:
    virtual AgentMapInterface::Iterator begin() override;
    virtual AgentMapInterface::ConstIterator constBegin() const override;

public:
    virtual AgentMapInterface::Iterator end() override;
    virtual AgentMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(AgentMapInterface::Name const &name) override;
    virtual bool add(const AgentMapInterface::Name &name, const Item *const item) override;

public:
    virtual bool remove(AgentMapInterface::Name const &name) override;

public:
    virtual AgentMapInterface::Item *getItem(AgentMapInterface::Name const &name) const override;

public:
    virtual void setName(AgentMapInterface::Item *const item,
                         AgentMapInterface::Name const &name) override;

    virtual AgentMapInterface::Name getName(AgentMapInterface::Item *const item) const override;

public:
    virtual bool contains(AgentMapInterface::Name const &name) const override;
    virtual int count() const override;

public:
    virtual QStringList keys() const override;
    virtual QList<Item *> values() const override;

protected:
    AgentMapInterface::Map agents; //!< the map (container) object for agent profiles
};

#endif // AGENTMAPMODEL_H
