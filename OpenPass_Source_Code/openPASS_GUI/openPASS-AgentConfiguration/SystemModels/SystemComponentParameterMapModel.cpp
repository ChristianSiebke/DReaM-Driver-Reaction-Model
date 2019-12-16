#include "SystemComponentParameterMapModel.h"

#include "SystemComponentParameterItemModel.h"

#include <iostream>

SystemComponentParameterMapModel::SystemComponentParameterMapModel(ComponentParameterMapInterface const * const _parameters,
                                                                   QObject * const parent)
    : SystemComponentParameterMapInterface(parent)
{
    for (ComponentParameterItemInterface const * const parameter : *_parameters)
    {
        parameters.insert(parameter->getID(), new SystemComponentParameterItemModel(parameter, this));
    }

    // connect SystemComponentParameterMapInterface to ComponentParameterMapInterface
    connect( _parameters, &ComponentParameterMapInterface::added, this, &SystemComponentParameterMapModel::add);
    connect( _parameters, &ComponentParameterMapInterface::removed, this, &SystemComponentParameterMapModel::remove);
}

void SystemComponentParameterMapModel::add(ComponentParameterItemInterface const * const componentParameter)
{
    SystemComponentParameterItemModel * parameter = new SystemComponentParameterItemModel(componentParameter,this);

    parameters.insert(componentParameter->getID(), parameter);
    Q_EMIT added();
}

void SystemComponentParameterMapModel::remove(ComponentParameterMapInterface::Item const * const componentParameter)
{
    ComponentParameterItemInterface::ID id = componentParameter->getID();

    // delete the input
    delete parameters.take(id);

    // inform the views that input has been removed
    Q_EMIT removed();
}

int SystemComponentParameterMapModel::count() const
{
    return parameters.count();
}

SystemComponentParameterMapInterface::Iterator SystemComponentParameterMapModel::begin()
{
    return parameters.begin();
}

SystemComponentParameterMapInterface::ConstIterator SystemComponentParameterMapModel::begin() const
{
    return parameters.begin();
}

SystemComponentParameterMapInterface::Iterator SystemComponentParameterMapModel::end()
{
    return parameters.end();
}

SystemComponentParameterMapInterface::ConstIterator SystemComponentParameterMapModel::end() const
{
    return parameters.end();
}

SystemComponentParameterMapInterface::ID SystemComponentParameterMapModel::getID(SystemComponentParameterMapInterface::Item * const item) const
{
    return parameters.key(item);
}

SystemComponentParameterMapInterface::Item * SystemComponentParameterMapModel::getItem(SystemComponentParameterMapInterface::ID const & id) const
{
    return parameters.value(id, nullptr);
}


QList<SystemComponentParameterMapInterface::Item*> SystemComponentParameterMapModel::values() const
{
    return parameters.values();
}
