#include "SystemComponentItemModel.h"

#include "SystemComponentInputMapModel.h"
#include "SystemComponentMapModel.h"
#include "SystemComponentOutputMapModel.h"
#include "SystemComponentParameterMapModel.h"
#include "SystemComponentScheduleModel.h"

SystemComponentItemModel::SystemComponentItemModel(ComponentItemInterface const * const component,
                                                   SystemComponentItemInterface::Position const & position,
                                                   QObject * const parent)
    : SystemComponentItemInterface(parent)
    , component(component)
    , schedule(new SystemComponentScheduleModel(component->getSchedule(), this))
    , parameters(new SystemComponentParameterMapModel(component->getParameters(), this))
    , inputs(new SystemComponentInputMapModel(component->getInputs(), this))
    , outputs(new SystemComponentOutputMapModel(component->getOutputs(), this))
    , name(component->getName())
    , type(component->getType())
    , title(component->getTitle())
    , position(position)
{
    // Connect SystemComponentItemInterface with ComponenItemInterface
    connect(component, &ComponentItemInterface::modifiedTitle, this, &SystemComponentItemModel::modifyTitle);
    connect(component, &ComponentItemInterface::modifiedName, this, &SystemComponentItemModel::modifyName);

    // Inform the component map that a component has been modified when in- and outputs have been deleted
    connect(inputs,&SystemComponentInputMapInterface::removed,
            qobject_cast<SystemComponentMapInterface * const> (parent), &SystemComponentMapInterface::modified);

    connect(outputs,&SystemComponentOutputMapInterface::removed,
            qobject_cast<SystemComponentMapInterface * const> (parent), &SystemComponentMapInterface::modified);
    }


SystemComponentItemModel::SystemComponentItemModel(SystemComponentItemModel const & component,
                                                   SystemComponentItemInterface::Position const & position,
                                                   QObject * const parent)
    : SystemComponentItemModel(component.component, position, parent)
{
    title = component.title;
    schedule->setPriority(component.schedule->getPriority());
    schedule->setOffset(component.schedule->getOffset());
    schedule->setCycle(component.schedule->getCycle());
    schedule->setResponse(component.schedule->getResponse());
    for (SystemComponentParameterItemInterface const * const parameter : *component.parameters)
    {
        parameters->getItem(parameter->getID())->setValue(parameter->getValue());
    }
}

void SystemComponentItemModel::modifyTitle()
{
    setTitle(component->getTitle());
}


void SystemComponentItemModel::modifyName()
{
    name=component->getName();
}

bool SystemComponentItemModel::duplicate(SystemComponentItemInterface::Position const & position)
{
    SystemComponentMapModel * const components = getComponents();
    return ((components) ? components->duplicate(this, position) : false);
}

bool SystemComponentItemModel::remove()
{
    SystemComponentMapModel * const components = getComponents();
    return ((components) ? components->remove(components->getID(this)) : false);
}

SystemComponentItemInterface::ID SystemComponentItemModel::getID() const
{
    SystemComponentMapModel const * const components = getComponents();
    return ((components) ? components->getID(const_cast<SystemComponentItemModel *>(this))
                         : SystemComponentItemInterface::ID());
}

SystemComponentItemInterface::Library SystemComponentItemModel::getLibrary() const
{
    return name;
}


SystemComponentItemInterface::Type SystemComponentItemModel::getType() const
{
    return type;
}

bool SystemComponentItemModel::setTitle(SystemComponentItemInterface::Title const & _title)
{
    title = _title;
    Q_EMIT modifiedTitle();
    return true;
}

SystemComponentItemInterface::Title SystemComponentItemModel::getTitle() const
{
    return title;
}

SystemComponentItemInterface::Schedule * SystemComponentItemModel::getSchedule() const
{
    return schedule;
}

SystemComponentItemInterface::ParameterMap * SystemComponentItemModel::getParameters() const
{
    return parameters;
}

SystemComponentItemInterface::InputMap * SystemComponentItemModel::getInputs() const
{
    return inputs;
}

SystemComponentItemInterface::OutputMap * SystemComponentItemModel::getOutputs() const
{
    return outputs;
}

bool SystemComponentItemModel::setPosition(SystemComponentItemInterface::Position const & _position)
{
    position = _position;
    Q_EMIT modifiedPosition();
    return true;
}

SystemComponentItemInterface::Position SystemComponentItemModel::getPosition() const
{
    return position;
}

// Get access to the components contained in the parent map
SystemComponentMapModel * SystemComponentItemModel::getComponents() const
{
    SystemComponentMapModel * components = nullptr;
    if (parent())
    {
        components = qobject_cast<SystemComponentMapModel * const>(parent());
    }
    return components;
}
