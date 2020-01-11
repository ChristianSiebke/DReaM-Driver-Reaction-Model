
#include "SubsystemItemModel.h"

#include "SystemComponentMapModel.h"
#include "SystemConnectionItemModel.h"
#include "SystemConnectionMapModel.h"
#include "SubsystemInputConnectionItemModel.h"
#include "SubsystemInputConnectionMapModel.h"
#include "SubsystemOutputConnectionItemModel.h"
#include "SubsystemOutputConnectionMapModel.h"
#include "SubsystemInputMapModel.h"
#include "SubsystemOutputMapModel.h"
#include "SubsystemParameterMapModel.h"

#include "SubsystemMapModel.h"

#include <iostream>

SubsystemItemModel::SubsystemItemModel(ComponentItemInterface * const component, QObject * const parent)
    : SubsystemItemInterface(parent)
    , title(component->getName())
    , priority(0)
    , connections(new SystemConnectionMapModel(this))
    , inputconnections(new SubsystemInputConnectionMapModel(this))
    , outputconnections(new SubsystemOutputConnectionMapModel(this))
    , components(new SystemComponentMapModel(connections, this))
    , component(component)
    , inputs(new SubsystemInputMapModel(component->getInputs(),this))
    , outputs(new SubsystemOutputMapModel(component->getOutputs(),this))
    , parameters(new SubsystemParameterMapModel(component->getParameters(),this))
{
}

SubsystemItemInterface::ID SubsystemItemModel::getID() const
{
    SubsystemMapModel const * const subsystems =
            qobject_cast<SubsystemMapModel const * const>(parent());
    return ((subsystems) ? subsystems->getID(const_cast<SubsystemItemModel *>(this))
                      : SubsystemItemInterface::ID());
}

bool SubsystemItemModel::setTitle(SubsystemItemInterface::Title const & _title)
{

    // change title of the Subsystem
    title = _title;

    Q_EMIT modifiedTitle();

    return true;
}

SubsystemItemInterface::Title SubsystemItemModel::getTitle() const
{
    return title;
}

bool SubsystemItemModel::setPriority(SubsystemItemInterface::Priority const & _priority)
{
    priority = _priority;
    Q_EMIT modifiedPriority();
    return true;
}

SubsystemItemInterface::Priority SubsystemItemModel::getPriority() const
{
    return priority;
}

SubsystemItemInterface::ComponentMap * SubsystemItemModel::getComponents() const
{
    return components;
}

SubsystemItemInterface::ConnectionMap * SubsystemItemModel::getConnections() const
{
    return connections;
}

SubsystemItemInterface::InputConnectionMap * SubsystemItemModel::getInputConnections() const
{
    return inputconnections;
}

SubsystemItemInterface::OutputConnectionMap * SubsystemItemModel::getOutputConnections() const
{
    return outputconnections;
}

SubsystemItemInterface::InputMap * SubsystemItemModel::getInputs() const
{
    return inputs;
}

SubsystemItemInterface::OutputMap * SubsystemItemModel::getOutputs() const
{
    return outputs;
}

SubsystemItemInterface::ParameterMap * SubsystemItemModel::getParameters() const
{
    return parameters;
}


QList<SystemConnectionMapInterface::Source const *> SubsystemItemModel::getCompatibleSources(
        SystemConnectionMapInterface::Target const * const target)
{
    QList<SystemConnectionMapInterface::Source const *> sources;
    for (SystemComponentItemInterface const * const component : *components)
    {
        for (SystemComponentOutputItemInterface const * const output : *component->getOutputs())
        {
            if ((SystemConnectionItemModel::isCompatible(output, target)) &&
                    (!connections->contains(output, target)))
            {
                sources << output;
            }
        }
    }
    return sources;
}

QList<SystemConnectionMapInterface::Target const *> SubsystemItemModel::getCompatibleTargets(
        SystemConnectionMapInterface::Source const * const source)
{
    QList<SystemConnectionMapInterface::Target const *> targets;
    for (SystemComponentItemInterface const * const component : *components)
    {
        for (SystemComponentInputItemInterface const * const input : *component->getInputs())
        {
            if ((SystemConnectionItemModel::isCompatible(source, input)) &&
                    (!connections->contains(source, input)))
            {
                targets << input;
            }
        }
    }
    return targets;
}


QList<SubsystemInputConnectionMapInterface::Source const *> SubsystemItemModel::getCompatibleInputSources(
        SubsystemInputConnectionMapInterface::Target const * const target)
{
    QList<SubsystemInputConnectionMapInterface::Source const *> sources;
    for (SubsystemInputItemInterface const * const input : *inputs)
    {
        if ((SubsystemInputConnectionItemModel::isCompatible(input, target)) &&
            (!inputconnections->contains(input, target)))
            {
                sources << input;
            }
    }
    return sources;
}


QList<SubsystemInputConnectionMapInterface::Target const *> SubsystemItemModel::getCompatibleInputTargets(
        SubsystemInputConnectionMapInterface::Source const * const source)
{
    QList<SubsystemInputConnectionMapInterface::Target const *> targets;
    for (SystemComponentItemInterface const * const component : *components)
    {
        for (SystemComponentInputItemInterface const * const input : *component->getInputs())
        {
            if ((SubsystemInputConnectionItemModel::isCompatible(source, input)) &&
                    (!inputconnections->contains(source, input)))
            {
                targets << input;
            }
        }
    }
    return targets;
}

QList<SubsystemOutputConnectionMapInterface::Source const *> SubsystemItemModel::getCompatibleOutputSources(
        SubsystemOutputConnectionMapInterface::Target const * const target)
{
    QList<SubsystemOutputConnectionMapInterface::Source const *> sources;
    for (SystemComponentItemInterface const * const component : *components)
    {
        for (SystemComponentOutputItemInterface const * const output : *component->getOutputs())
        {
            if ((SubsystemOutputConnectionItemModel::isCompatible(output, target)) &&
                    (!outputconnections->contains(output, target)))
            {
                sources << output;
            }
        }
    }
    return sources;
}


QList<SubsystemOutputConnectionMapInterface::Target const *> SubsystemItemModel::getCompatibleOutputTargets(
        SubsystemOutputConnectionMapInterface::Source const * const source)
{
    QList<SubsystemOutputConnectionMapInterface::Target const *> targets;
    for (SubsystemOutputItemInterface const * const output : *outputs)
    {

        if ((SubsystemOutputConnectionItemModel::isCompatible(source, output)) &&
            (!outputconnections->contains(source, output)))
            {
                targets << output;
            }
    }
    return targets;
}
