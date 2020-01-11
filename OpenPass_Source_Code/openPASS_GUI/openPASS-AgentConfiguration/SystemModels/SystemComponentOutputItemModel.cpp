#include "SystemComponentOutputItemModel.h"

#include "openPASS-System/SystemComponentItemInterface.h"
#include "SystemComponentOutputMapModel.h"

SystemComponentOutputItemModel::SystemComponentOutputItemModel(ComponentOutputItemInterface const * const output,
                                                               QObject * const parent)
    : SystemComponentOutputItemInterface(parent)
    , output(output)
    , type(output->getType())
    , title(output->getTitle())
    , unit(output->getUnit())
{
    connect(output, &ComponentOutputItemInterface::modifiedTitle, this, &SystemComponentOutputItemModel::modifyTitle);
    connect(output, &ComponentOutputItemInterface::modifiedType, this, &SystemComponentOutputItemModel::modifyType);
    connect(output, &ComponentOutputItemInterface::modifiedUnit, this, &SystemComponentOutputItemModel::modifyUnit);
}

void SystemComponentOutputItemModel::modifyTitle()
{
    title=output->getTitle();
    Q_EMIT modifiedTitle();
}

void SystemComponentOutputItemModel::modifyType()
{
    type=output->getType();
    Q_EMIT modifiedType();
}


void SystemComponentOutputItemModel::modifyUnit()
{
    unit=output->getUnit();
    Q_EMIT modifiedUnit();
}

SystemComponentItemInterface * SystemComponentOutputItemModel::getComponent() const
{
    if ((parent()) && (parent()->parent()))
    {
        SystemComponentItemInterface * const item =
                qobject_cast<SystemComponentItemInterface * const>(parent()->parent());
        return ((item) ? item : nullptr);
    }
    return nullptr;
}

SystemComponentOutputItemInterface::ID SystemComponentOutputItemModel::getID() const
{
    SystemComponentOutputMapModel const * const outputs =
            qobject_cast<SystemComponentOutputMapModel const * const>(parent());
    return ((outputs) ? outputs->getID(const_cast<SystemComponentOutputItemModel *>(this))
                      : SystemComponentOutputItemInterface::ID());
}

SystemComponentOutputItemInterface::Type SystemComponentOutputItemModel::getType() const
{
    return type;
}

SystemComponentOutputItemInterface::Title SystemComponentOutputItemModel::getTitle() const
{
    return title;
}

SystemComponentOutputItemInterface::Unit SystemComponentOutputItemModel::getUnit() const
{
    return unit;
}
