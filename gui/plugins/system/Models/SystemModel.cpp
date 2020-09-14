/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

#include "Models/SystemModel.h"

#include <QCoreApplication>

#include "Models/SystemComponentManagerModel.h"
#include "Models/SystemMapModel.h"
#include "Models/SystemXMLLoadModel.h"
#include "Models/SystemXMLSaveModel.h"

SystemModel::SystemModel(ComponentInterface *const component,
                         QObject *const parent) :
    SystemInterface(parent), component(component), components(new SystemComponentManagerModel(component, this)), systems(new SystemMapModel(this))
{
    // Load components
    components->loadFromDirectory(QDir(QCoreApplication::applicationDirPath() + SUBDIR_LIB_COMPONENTS));
}

bool SystemModel::clear()
{
    if (systems->clear())
    {
        Q_EMIT cleared();
        return true;
    }
    return false;
}

bool SystemModel::load(QString const &filepath, bool const *const dynamicMode)
{
    clear();
    if (SystemXMLLoadModel::load(filepath, systems, components, dynamicMode))
    {
        Q_EMIT loaded();
        return true;
    }
    return false;
}

bool SystemModel::load(QIODevice *const device, bool const *const dynamicMode)
{
    clear();
    if (SystemXMLLoadModel::load(device, systems, components, dynamicMode))
    {
        Q_EMIT loaded();
        return true;
    }
    return false;
}

bool SystemModel::save(QString const &filepath, bool const *const dynamicMode) const
{
    if (SystemXMLSaveModel::save(filepath, systems, dynamicMode))
    {
        Q_EMIT saved();
        return true;
    }
    return false;
}

bool SystemModel::save(QIODevice *const device, bool const *const dynamicMode) const
{
    if (SystemXMLSaveModel::save(device, systems, dynamicMode))
    {
        Q_EMIT saved();
        return true;
    }
    return false;
}

SystemComponentManagerInterface *SystemModel::getComponents() const
{
    return components;
}

SystemMapInterface *SystemModel::getSystems() const
{
    return systems;
}

SystemInterface *SystemModel::createSystemModel()
{
    return new SystemModel(component);
}
