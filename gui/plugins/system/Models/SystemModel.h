/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef SYSTEMMODEL_H
#define SYSTEMMODEL_H

#include "openPASS-Project/ProjectInterface.h"
#include "openPASS-System/SystemInterface.h"

class ComponentInterface;
class ProjectInterface;
class SystemComponentManagerModel;

class SystemModel : public SystemInterface
{
    Q_OBJECT

public:
    explicit SystemModel(ComponentInterface *const component,
                         QObject *const parent = nullptr);
    virtual ~SystemModel() = default;

public:
    virtual bool clear() override;
    virtual bool load(QString const &filepath, bool const *const dynamicMode) override;
    virtual bool load(QIODevice *const device, bool const *const dynamicMode) override;
    virtual bool save(QString const &filepath, bool const *const dynamicMode) const override;
    virtual bool save(QIODevice *const device, bool const *const dynamicMode) const override;

public:
    virtual SystemComponentManagerInterface *getComponents() const override;
    virtual SystemMapInterface *getSystems() const override;

public:
    virtual SystemInterface *createSystemModel() override;

protected:
    ComponentInterface *const component;
    SystemComponentManagerInterface *const components;
    SystemMapInterface *const systems;
};

#endif // SYSTEMMODEL_H
