#include "SystemComponentScheduleModel.h"

#include "SystemComponentItemModel.h"
#include "SystemComponentMapModel.h"

SystemComponentScheduleModel::SystemComponentScheduleModel(ComponentScheduleInterface const * const schedule,
                                                           QObject * const parent)
    : SystemComponentScheduleInterface(parent)
    , priority(0)
    , offset(schedule->getOffset())
    , cycle(schedule->getCycle())
    , response(schedule->getResponse())
{
}

bool SystemComponentScheduleModel::setPriority(SystemComponentScheduleInterface::Priority const & _priority)
{
    if (checkValidPriority(_priority))
    {
        priority = _priority;
    }
    else
    {
        priority = 0;
    }
    Q_EMIT modifiedPriority();
    return true;
}

bool SystemComponentScheduleModel::checkValidPriority(SystemComponentScheduleInterface::Priority const &priority)
{
    // Bug fix cancelling the verification of the unicity of the value priority in the list of components modelling the system
        Q_UNUSED(priority);
        return true;
}

SystemComponentScheduleInterface::Priority SystemComponentScheduleModel::getPriority() const
{
    return priority;
}

bool SystemComponentScheduleModel::setOffset(SystemComponentScheduleInterface::Offset const & _offset)
{
    offset = _offset;
    Q_EMIT modifiedOffset();
    return true;
}

SystemComponentScheduleInterface::Offset SystemComponentScheduleModel::getOffset() const
{
    return offset;
}

bool SystemComponentScheduleModel::setCycle(SystemComponentScheduleInterface::Cycle const & _cycle)
{
    cycle = _cycle;
    Q_EMIT modifiedCycle();
    return true;
}

SystemComponentScheduleInterface::Cycle SystemComponentScheduleModel::getCycle() const
{
    return cycle;
}

bool SystemComponentScheduleModel::setResponse(SystemComponentScheduleInterface::Response const & _response)
{
    response = _response;
    Q_EMIT modifiedResponse();
    return true;
}

SystemComponentScheduleInterface::Response SystemComponentScheduleModel::getResponse() const
{
    return response;
}
