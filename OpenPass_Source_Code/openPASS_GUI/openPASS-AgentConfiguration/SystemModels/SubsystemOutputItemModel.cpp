
#include "SubsystemOutputItemModel.h"

#include "SubsystemOutputMapModel.h"

SubsystemOutputItemInterface::Type const SubsystemOutputItemModel::DefaultType = QStringLiteral("double");
SubsystemOutputItemInterface::Title const SubsystemOutputItemModel::DefaultTitle = QStringLiteral("Output");
SubsystemOutputItemInterface::Unit const SubsystemOutputItemModel::DefaultUnit = QStringLiteral("");

SubsystemOutputItemModel::SubsystemOutputItemModel(ComponentOutputItemInterface * const componentOutputItem, QObject * const parent)
    : SubsystemOutputItemInterface(parent)
    , componentOutputItem(componentOutputItem)
    , type(DefaultType)
    , title(DefaultTitle)
    , unit(DefaultUnit)
{
    componentOutputItem->setType(DefaultType);
    componentOutputItem->setTitle(DefaultTitle);
    componentOutputItem->setUnit(DefaultUnit);
}

bool SubsystemOutputItemModel::clear()
{
    setType(DefaultType);
    setTitle(DefaultTitle);
    setUnit(DefaultUnit);
    componentOutputItem->setType(DefaultType);
    componentOutputItem->setTitle(DefaultTitle);
    componentOutputItem->setUnit(DefaultUnit);
    Q_EMIT cleared();
    return true;
}

bool SubsystemOutputItemModel::setID(SubsystemOutputItemInterface::ID const & id)
{
    bool success;
    SubsystemOutputMapModel * const map =
            qobject_cast<SubsystemOutputMapModel * const>(parent());

    success = componentOutputItem->setID(id);

    return ((map) ? map->setID(const_cast<SubsystemOutputItemModel *>(this), id)
                  : false) && success;
}

SubsystemOutputItemInterface::ID SubsystemOutputItemModel::getID() const
{
    SubsystemOutputMapModel const * const map =
            qobject_cast<SubsystemOutputMapModel const * const>(parent());
    return ((map) ? map->getID(const_cast<SubsystemOutputItemModel *>(this))
                  : SubsystemOutputItemInterface::ID());
}

bool SubsystemOutputItemModel::setType(SubsystemOutputItemInterface::Type const & _type)
{
    type = _type;
    componentOutputItem->setType(_type);
    Q_EMIT modifiedType();
    return true;
}

SubsystemOutputItemInterface::Type SubsystemOutputItemModel::getType() const
{
    return type;
}

bool SubsystemOutputItemModel::setTitle(SubsystemOutputItemInterface::Title const & _title)
{
    title = _title;
    componentOutputItem->setTitle(_title);
    Q_EMIT modifiedTitle();
    return true;
}

SubsystemOutputItemInterface::Title SubsystemOutputItemModel::getTitle() const
{
    return title;
}

bool SubsystemOutputItemModel::setUnit(SubsystemOutputItemInterface::Unit const & _unit)
{
    unit = _unit;
    componentOutputItem->setUnit(_unit);
    Q_EMIT modifiedUnit();
    return true;
}

SubsystemOutputItemInterface::Unit SubsystemOutputItemModel::getUnit() const
{
    return unit;
}

SubsystemOutputItemInterface::ComponentOutputItem * SubsystemOutputItemModel::getComponentOutput() const
{
    return componentOutputItem;
}
