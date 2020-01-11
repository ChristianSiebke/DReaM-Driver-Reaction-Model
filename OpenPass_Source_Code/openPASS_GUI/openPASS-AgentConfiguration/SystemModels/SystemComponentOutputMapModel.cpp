#include "SystemComponentOutputMapModel.h"

#include "SystemComponentOutputItemModel.h"

SystemComponentOutputMapModel::SystemComponentOutputMapModel(ComponentOutputMapInterface const * const _outputs,
                                                             QObject * const parent)
    : SystemComponentOutputMapInterface(parent)
{
    for (ComponentOutputItemInterface const * const output : *_outputs)
    {
        outputs.insert(output->getID(), new SystemComponentOutputItemModel(output, this));
    }

    // Connect SystemComponentOutputMapInterface to ComponentOutputMapInterface
    connect(_outputs,&ComponentOutputMapInterface::added,this, &SystemComponentOutputMapModel::add);
    connect(_outputs,&ComponentOutputMapInterface::removed,this, &SystemComponentOutputMapModel::remove);
}

void SystemComponentOutputMapModel::add(ComponentOutputItemInterface const * const componentOutput)
{
    SystemComponentOutputItemModel * output = new SystemComponentOutputItemModel(componentOutput,this);
    outputs.insert(componentOutput->getID(), output);
    Q_EMIT added();
}

void SystemComponentOutputMapModel::remove(ComponentOutputMapInterface::Item const * const componentOutput)
{
    ComponentOutputItemInterface::ID id = componentOutput->getID();

    // trigger the signal to remove connection in which output is involved
    Q_EMIT outputs.value(id)->removeConnection(outputs.value(id));

    // delete the output
    delete outputs.take(id);

    // inform view that output has been removed
    Q_EMIT removed();
}

SystemComponentOutputMapInterface::Iterator SystemComponentOutputMapModel::begin()
{
    return outputs.begin();
}

SystemComponentOutputMapInterface::ConstIterator SystemComponentOutputMapModel::begin() const
{
    return outputs.begin();
}

bool SystemComponentOutputMapModel::contains(SystemComponentOutputMapInterface::ID const &id) const
{
    return outputs.keys().contains(id);
}

SystemComponentOutputMapInterface::Iterator SystemComponentOutputMapModel::end()
{
    return outputs.end();
}

SystemComponentOutputMapInterface::ConstIterator SystemComponentOutputMapModel::end() const
{
    return outputs.end();
}

SystemComponentOutputMapInterface::ID SystemComponentOutputMapModel::getID(SystemComponentOutputMapInterface::Item * const item) const
{
    return outputs.key(item);
}

SystemComponentOutputMapInterface::Item * SystemComponentOutputMapModel::getItem(SystemComponentOutputMapInterface::ID const & id) const
{
    return outputs.value(id, nullptr);
}
