#include "SystemComponentInputMapModel.h"

#include "SystemComponentInputItemModel.h"

#include "SystemComponentMapModel.h"

SystemComponentInputMapModel::SystemComponentInputMapModel(ComponentInputMapInterface const * const _inputs,
                                                           QObject * const parent)
    : SystemComponentInputMapInterface(parent)
{
    for (ComponentInputItemInterface const * const input : *_inputs)
    {
        inputs.insert(input->getID(), new SystemComponentInputItemModel(input, this));
    }

    // Connect SystemComponentInputMapInterface to ComponentInputMapInterface
    connect(_inputs,&ComponentInputMapInterface::added,this, &SystemComponentInputMapModel::add);
    connect(_inputs,&ComponentInputMapInterface::removed,this, &SystemComponentInputMapModel::remove);
}

void SystemComponentInputMapModel::add(ComponentInputItemInterface const * const componentInput)
{
    SystemComponentInputItemModel * input = new SystemComponentInputItemModel(componentInput,this);
    inputs.insert(componentInput->getID(), input);
    Q_EMIT added();
}

void SystemComponentInputMapModel::remove(ComponentInputMapInterface::Item const * const componentInput)
{
    ComponentInputItemInterface::ID id = componentInput->getID();

    // trigger signal to remove connections in which this input item is involved
    Q_EMIT inputs.value(id)->removeConnection(inputs.value(id));

    // delete the input
    delete inputs.take(id);

    // inform the views that input has been removed
    Q_EMIT removed();
}

SystemComponentInputMapInterface::Iterator SystemComponentInputMapModel::begin()
{
    return inputs.begin();
}

SystemComponentInputMapInterface::ConstIterator SystemComponentInputMapModel::begin() const
{
    return inputs.begin();
}

bool SystemComponentInputMapModel::contains(SystemComponentInputMapInterface::ID const &id) const
{
    return inputs.keys().contains(id);
}

SystemComponentInputMapInterface::Iterator SystemComponentInputMapModel::end()
{
    return inputs.end();
}

SystemComponentInputMapInterface::ConstIterator SystemComponentInputMapModel::end() const
{
    return inputs.end();
}

SystemComponentInputMapInterface::ID SystemComponentInputMapModel::getID(SystemComponentInputMapInterface::Item * const item) const
{
    return inputs.key(item);
}

SystemComponentInputMapInterface::Item * SystemComponentInputMapModel::getItem(SystemComponentInputMapInterface::ID const & id) const
{
    return inputs.value(id, nullptr);
}
