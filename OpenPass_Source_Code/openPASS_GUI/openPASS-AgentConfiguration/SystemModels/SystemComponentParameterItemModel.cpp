#include "openPASS-System/SystemComponentItemInterface.h"
#include "SystemComponentParameterItemModel.h"
#include "SystemComponentParameterMapModel.h"

#include <iostream>

SystemComponentParameterItemModel::SystemComponentParameterItemModel(ComponentParameterItemInterface const * const parameter,
                                                                     QObject * const parent)
    : SystemComponentParameterItemInterface(parent)
    , parameter(parameter)
    , type(parameter->getType())
    , title(parameter->getTitle())
    , unit(parameter->getUnit())
    , value(parameter->getValue())
{
    connect(parameter, &ComponentParameterItemInterface::modifiedTitle, this, &SystemComponentParameterItemModel::modifyTitle);
}

void SystemComponentParameterItemModel::modifyTitle()
{
    title=parameter->getTitle();
    Q_EMIT modifiedTitle();
}

SystemComponentParameterItemInterface::ID SystemComponentParameterItemModel::getID() const
{
    SystemComponentParameterMapModel const * const parameters =
            qobject_cast<SystemComponentParameterMapModel const * const>(parent());
    return ((parameters) ? parameters->getID(const_cast<SystemComponentParameterItemModel *>(this))
                         : SystemComponentParameterItemInterface::ID());
}

SystemComponentParameterItemInterface::Type SystemComponentParameterItemModel::getType() const
{
    return type;
}

SystemComponentParameterItemInterface::Title SystemComponentParameterItemModel::getTitle() const
{
    return title;
}

SystemComponentParameterItemInterface::Unit SystemComponentParameterItemModel::getUnit() const
{
    return unit;
}

bool SystemComponentParameterItemModel::setValue(SystemComponentParameterItemInterface::Value const & _value)
{
    value = _value;
    Q_EMIT modifiedValue();
    return true;
}

SystemComponentParameterItemInterface::Value SystemComponentParameterItemModel::getValue() const
{
    return value;
}

SystemComponentItemInterface * SystemComponentParameterItemModel::getComponent() const
{
    SystemComponentItemInterface * const component = qobject_cast< SystemComponentItemInterface * const>(parent()->parent());

    return component;
    //return ( (component) ? component : nullptr );
}
