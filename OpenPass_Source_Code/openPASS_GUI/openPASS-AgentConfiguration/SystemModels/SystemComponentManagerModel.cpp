#include "SystemComponentManagerModel.h"

#include <QList>
#include <QString>

SystemComponentManagerModel::SystemComponentManagerModel(ComponentInterface * const component,
                                                         QObject * const parent)
    : SystemComponentManagerInterface(parent)
    , components(component->createComponentMap(this))
{
}

bool SystemComponentManagerModel::loadFromDirectory(QDir const & directory)
{
    // Load the component file names and remove the xml extension
    QList<QString> componentFiles = directory.entryList({"*.xml"},QDir::Files | QDir::Readable, QDir::Name);

    // Load the components and update the component Map
    for (QString filename : componentFiles)
    {
        components->add(filename.remove(filename.count() - 4, 4),
                        directory.absoluteFilePath(filename));
    }

    return true;
}

QList<ComponentItemInterface::Title> SystemComponentManagerModel::listTitlesByType(ComponentItemInterface::Type const & type) const
{
    QList<ComponentItemInterface::Title> titles;
    for (ComponentMapInterface::Item * const item : *components)
    {
        if (item->getType() == type)
        {
            titles << item->getTitle();
        }
    }
    return titles;
}

ComponentItemInterface * SystemComponentManagerModel::lookupItemByName(ComponentItemInterface::Name const & name) const
{
    for (ComponentMapInterface::Item * const item : *components)
    {
        if (item->getName() == name)
        {
            return item;
        }
    }
    return nullptr;
}

ComponentItemInterface * SystemComponentManagerModel::lookupItemByTitle(ComponentItemInterface::Title const & title) const
{
    for (ComponentMapInterface::Item * const item : *components)
    {
        if (item->getTitle() == title)
        {
            return item;
        }
    }
    return nullptr;
}

ComponentMapInterface * SystemComponentManagerModel::getComponents() const
{
    return components;
}
