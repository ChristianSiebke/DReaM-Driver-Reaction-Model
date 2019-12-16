#include "SystemModel.h"

#include "openPASS-Project/ProjectInterface.h"
#include "SystemComponentManagerModel.h"
#include "SystemMapModel.h"
#include "SubsystemMapModel.h"
#include "SystemXMLLoadModel.h"
#include "SystemXMLSaveModel.h"

SystemModel::SystemModel(ComponentInterface * const component,
                         ProjectInterface * const project,
                         QObject * const parent)
    : SystemInterface(parent)
    , components(new SystemComponentManagerModel(component, this))
    , _project(project)
    , systems(new SystemMapModel(this))
    , subsystems(new SubsystemMapModel(components->getComponents(),this))
{
    // Load components
    components->loadFromDirectory(_project->getLibrary());

    connect(subsystems,&SubsystemMapInterface::removed,this,&SystemModel::removeSubsystemComponentItems);
}

void SystemModel::removeSubsystemComponentItems(SystemComponentItemInterface::Library library)
{
    // remove all occurences of SubsystemComponent in Systems
    for(auto system : systems->values())
        for(auto component : system->getComponents()->values() )
        {
            if(component->getLibrary() == library)
                component->remove();
        }

    // remove all occurences of SubsystemComponent in Subsystems
    for(auto subsystem : subsystems->values())
        for(auto component : subsystem->getComponents()->values() )
        {
            if(component->getLibrary() == library)
                component->remove();
        }
}


bool SystemModel::clear()
{
    if (systems->clear() && subsystems->clear())
    {
        return true;
    }
    return false;
}


bool SystemModel::load(QString const & filepath)
{
    clear();
    if (SystemXMLLoadModel::load(filepath, systems, subsystems, components))
    {
        Q_EMIT loaded();
        return true;
    }
    return false;
}


bool SystemModel::save(QString const & filepath) const
{
    if (SystemXMLSaveModel::save(filepath, systems,subsystems))
    {
        Q_EMIT saved();
        return true;
    }
    return false;
}


SystemComponentManagerInterface * SystemModel::getComponents() const
{
    return components;
}

SystemMapInterface * SystemModel::getSystems() const
{
    return systems;
}

SubsystemMapInterface * SystemModel::getSubsystems() const
{
    return subsystems;
}

SystemModel::Filepath SystemModel::getCurrentFilepath() const
{
    return currentFilepath;
}

void SystemModel::setCurrentFilepath(const Filepath &_filepath)
{
    currentFilepath = _filepath;
}
