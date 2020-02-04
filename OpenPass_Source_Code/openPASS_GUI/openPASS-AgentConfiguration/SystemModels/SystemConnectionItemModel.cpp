#include "SystemConnectionItemModel.h"

#include "SystemConnectionMapModel.h"

SystemConnectionItemModel::SystemConnectionItemModel(SystemConnectionItemInterface::Source const * const source,
                                                     SystemConnectionItemInterface::Target const * const target,
                                                     QObject * const parent)
    : SystemConnectionItemInterface(parent)
    , source(source)
    , target(target)
{
}

SystemConnectionItemInterface::ID SystemConnectionItemModel::getID() const
{
    SystemConnectionMapModel const * const connections =
            qobject_cast<SystemConnectionMapModel const * const>(parent());
    return ((connections) ? connections->getID(const_cast<SystemConnectionItemModel *>(this))
                          : SystemConnectionItemInterface::ID());
}

SystemConnectionItemInterface::Source const * SystemConnectionItemModel::getSource() const
{
    return source;
}

SystemConnectionItemInterface::Target const * SystemConnectionItemModel::getTarget() const
{
    return target;
}

bool SystemConnectionItemModel::isCompatible() const
{
    return isCompatible(source, target);
}

bool SystemConnectionItemModel::isCompatible(SystemConnectionItemInterface::Source const * const source,
                                             SystemConnectionItemInterface::Target const * const target)
{
    return ((source->getComponent() != target->getComponent()) &&
            (source->getType() == target->getType()) &&
            ((source->getUnit() == target->getUnit()) ||
             (source->getUnit() == QStringLiteral("")) ||
             (target->getUnit() == QStringLiteral(""))));
}
