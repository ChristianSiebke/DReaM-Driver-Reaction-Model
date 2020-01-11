#include "SystemComponentInputItemModel.h"

#include "openPASS-System/SystemComponentItemInterface.h"
#include "SystemComponentInputMapModel.h"
#include "openPASS-System/SystemConnectionMapInterface.h"

SystemComponentInputItemModel::SystemComponentInputItemModel(ComponentInputItemInterface const * const input,
                                                             QObject * const parent)
    : SystemComponentInputItemInterface(parent)
    , input(input)
    , type(input->getType())
    , title(input->getTitle())
    , unit(input->getUnit())
    , cardinality(input->getCardinality())
{
    connect(input, &ComponentInputItemInterface::modifiedTitle, this, &SystemComponentInputItemModel::modifyTitle);
    connect(input, &ComponentInputItemInterface::modifiedType, this, &SystemComponentInputItemModel::modifyType);
    connect(input, &ComponentInputItemInterface::modifiedUnit, this, &SystemComponentInputItemModel::modifyUnit);
}

void SystemComponentInputItemModel::modifyTitle()
{

    title=input->getTitle();
    Q_EMIT modifiedTitle();
}

void SystemComponentInputItemModel::modifyType()
{
    type=input->getType();
    Q_EMIT modifiedType();
}


void SystemComponentInputItemModel::modifyUnit()
{
    unit=input->getUnit();
    Q_EMIT modifiedUnit();
}


SystemComponentItemInterface * SystemComponentInputItemModel::getComponent() const
{
    if ((parent()) && (parent()->parent()))
    {
        SystemComponentItemInterface * const item =
                qobject_cast<SystemComponentItemInterface * const>(parent()->parent());
        return ((item) ? item : nullptr);
    }
    return nullptr;
}


SystemComponentInputItemInterface::ID SystemComponentInputItemModel::getID() const
{
    SystemComponentInputMapModel const * const inputs =
            qobject_cast<SystemComponentInputMapModel const * const>(parent());
    return ((inputs) ? inputs->getID(const_cast<SystemComponentInputItemModel *>(this))
                     : SystemComponentInputItemInterface::ID());
}

SystemComponentInputItemInterface::Type SystemComponentInputItemModel::getType() const
{
    return type;
}

SystemComponentInputItemInterface::Title SystemComponentInputItemModel::getTitle() const
{
    return title;
}

SystemComponentInputItemInterface::Unit SystemComponentInputItemModel::getUnit() const
{
    return unit;
}

SystemComponentInputItemInterface::Cardinality SystemComponentInputItemModel::getCardinality() const
{
    return cardinality;
}
