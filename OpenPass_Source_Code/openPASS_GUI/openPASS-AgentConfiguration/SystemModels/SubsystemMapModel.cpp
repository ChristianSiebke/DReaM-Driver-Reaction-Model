#include "SubsystemMapModel.h"
#include "SubsystemItemModel.h"

SubsystemMapModel::SubsystemMapModel(ComponentMapInterface * const componentMap, QObject * const parent)
    : SubsystemMapInterface(parent)
    , componentMap(componentMap)
{
}

SubsystemMapInterface::Iterator SubsystemMapModel::begin()
{
    return subsystems.begin();
}

SubsystemMapInterface::ConstIterator SubsystemMapModel::begin() const
{
    return subsystems.begin();
}

bool SubsystemMapModel::clear()
{

    // delete all components of type "Subsystem" from component Map
    componentMap->remove(ComponentItemInterface::Type::Subsystem);

    // delete all subsystems
    qDeleteAll(subsystems);

    // clear the map
    subsystems.clear();

    Q_EMIT cleared();

    return true;
}

SubsystemMapInterface::Iterator SubsystemMapModel::end()
{
    return subsystems.end();
}

SubsystemMapInterface::ConstIterator SubsystemMapModel::end() const
{
    return subsystems.end();
}

SubsystemMapInterface::ID SubsystemMapModel::getID(SubsystemMapInterface::Item * const item) const
{
    return subsystems.key(item);
}

SubsystemMapInterface::Item * SubsystemMapModel::getItem(SubsystemMapInterface::ID const & id) const
{
    return subsystems.value(id, nullptr);
}

int SubsystemMapModel::count() const
{
    return subsystems.count();
}

bool SubsystemMapModel::remove(SubsystemMapInterface::ID const & id)
{
    if(subsystems.keys().contains(id))
    {

       SystemComponentItemInterface::Library library = subsystems.value(id)->getTitle();

       // send signal to remove the view first
       Q_EMIT removed(library);


       // remove subsystem component from Component map
       componentMap->remove(library);

       // delete the subsystem
       delete subsystems.take(id);

       return true;
    }
    return false;

}

bool SubsystemMapModel::add(const SubsystemMapInterface::ID &id, SubsystemMapInterface::Item::Title const & name)
{

    if (!contains(id))
    {
        // Create the subsystem
        componentMap->add(name);
        componentMap->getItem(name)->setType(ComponentItemInterface::Type::Subsystem);
        componentMap->getItem(name)->setTitle(name);

        SubsystemMapModel::Item *subsystem = new SubsystemItemModel(componentMap->getItem(name),this);
        subsystems.insert(id, subsystem);

        Q_EMIT added(subsystem);

        return true;
    }
    return false;
}

bool SubsystemMapModel::add(const SubsystemMapInterface::ID &id, SubsystemMapInterface::Item::Component * const component)
{

    if (!contains(id))
    {
        // Create the subsystem
        SubsystemMapModel::Item *subsystem = new SubsystemItemModel(component, this);
        subsystems.insert(id, subsystem);

        Q_EMIT added(subsystem);

        return true;
    }
    return false;
}

SubsystemItemInterface::Title SubsystemMapModel::generateTitle()
{
    int name_id=1;
    ComponentItemInterface::Title name = QString("Subsystem %1").arg(name_id);
    QList<ComponentItemInterface::Title> NameList;

    for (ComponentMapInterface::Item * const item : *componentMap)
    {
        if (item->getType() == ComponentMap::Item::Type::Subsystem)
        {
            NameList << item->getTitle();
        }
    }

    // Determine free name with pattern Subsystem X with X being the lowest unused number
    while( NameList.contains(name) )
    {
        name_id++;
        name = QString("Subsystem %1").arg(name_id);
    }

    return name;
}

// returns highest ID which exists in the System Map
SubsystemMapInterface::ID SubsystemMapModel::lastID()
{
    return subsystems.lastKey();
}

bool SubsystemMapModel::contains(SubsystemItemInterface::Title title)
{

    for(SubsystemMapModel::Item * item : subsystems)
    {
        if(item->getTitle() == title)
            return true;
    }

    return false;
}

bool SubsystemMapModel::contains(SubsystemItemInterface::ID const & id)
{
    return subsystems.contains(id);
}


QList <SubsystemMapInterface::Item *> SubsystemMapModel::values()
{
   return subsystems.values();
}

SubsystemMapInterface::ID SubsystemMapModel::generateID()
{
    SubsystemMapInterface::ID id = 1;
    for (SubsystemMapInterface::ID key : subsystems.keys())
    {
        if (id <= key)
        {
            id = key + 1;
        }
    }
    return id;
}
