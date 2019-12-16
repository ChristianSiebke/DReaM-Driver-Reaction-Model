#include "SystemErrorModel.h"

SystemErrorModel::SystemErrorModel(SystemInterface * const system,
                                   QObject * const parent)
    : SystemErrorInterface(parent)
    , system(system)
    , errors( {{Type::connection,{}}, {Type::input,{}}})
{
}

QString SystemErrorModel::getErrorReport()
{
    QString report;

    // If there are no errors...
    if(errors[Type::connection].isEmpty() && errors[Type::input].isEmpty())
    {
        report = QString("****** NO ERRORS ******");
        return report;
    }
    // If there are errors...
    else
    {
        // Put header to inform user that there are errors
        report =  QString("****** ERRORS DETECTED ******\n");

        // Display all connection type errors
        if(!errors[Type::connection].isEmpty())
        {
            // Start a list of infos on incompatible connections
            report.append(QString("\nIncompatible connections detected:\n") );

            // List incompatible connections in systems
            for(auto item : system->getSystems()->values())
            {
                QString pattern = QString("SYS %1: CON").arg(item->getID());
                int num_connections = errors[Type::connection].filter(pattern).count();

                if(num_connections!=0)
                    report.append(QString("* in System '%1': number of incompatible connections: %2\n").arg(item->getTitle()).arg(num_connections));
            }

            // List incompatible connections in subystems
            for(auto item : system->getSubsystems()->values())
            {
                QString pattern1 = QString("SUB %1: CON").arg(item->getID());
                QString pattern2 = QString("SUB %1: INCON").arg(item->getID());
                QString pattern3 = QString("SUB %1: OUTCON").arg(item->getID());

                int num_connections = errors[Type::connection].filter(pattern1).count();
                num_connections += errors[Type::connection].filter(pattern2).count();
                num_connections += errors[Type::connection].filter(pattern3).count();

                if(num_connections!=0)
                    report.append(QString("* in Subsystem '%1': number of incompatible connections: %2\n").arg(item->getTitle()).arg(num_connections));
            }
        }

        // Display all input type errors
        if(!errors[Type::input].isEmpty())
        {
            // Start a list of infos on incompatible connections
            report.append(QString("\nSystem components with unconnected inputs detected:\n") );

            for(auto item : system->getSystems()->values())
            {
                for(auto component : item->getComponents()->values())
                {

                    QString pattern = QString("SYS %1: IN: %2").arg(item->getID()).arg(component->getID());
                    int num_inputs = errors[Type::input].filter(pattern).count();

                    if(num_inputs!=0)
                        report.append(QString("* in System '%1': number of unconnected inputs in component '%2': %3\n")
                                      .arg(item->getTitle())
                                      .arg(component->getTitle())
                                      .arg(num_inputs));
                }

                for(auto item : system->getSubsystems()->values())
                {
                    for(auto component : item->getComponents()->values())
                    {

                        QString pattern = QString("SUB %1: IN: %2").arg(item->getID()).arg(component->getID());
                        int num_inputs = errors[Type::input].filter(pattern).count();

                        if(num_inputs!=0)
                            report.append(QString("* in Subsystem '%1': number of unconnected inputs in component '%2': %3\n")
                                          .arg(item->getTitle())
                                          .arg(component->getTitle())
                                          .arg(num_inputs));
                    }
                }
            }
        }

        return report;
    }
}

void SystemErrorModel::clearErrors()
{
    errors[Type::connection].clear();
    errors[Type::input].clear();
}

void SystemErrorModel::checkConnection(SystemConnectionItemInterface const * const connection)
{
    // Determine the parents (either a system or a subsystem)
    SystemItemInterface * system = qobject_cast<SystemItemInterface *>(connection->parent()->parent());
    SubsystemItemInterface * subsystem = qobject_cast<SubsystemItemInterface *>(connection->parent()->parent());

    // Is the parent a system?
    if(system)
    {
        // generate error code characterising the connection
        QString code = QString("SYS %1: CON: %2,%3;%4,%5")
                .arg(system->getID())
                .arg(connection->getSource()->getComponent()->getID())
                .arg(connection->getSource()->getID())
                .arg(connection->getTarget()->getComponent()->getID())
                .arg(connection->getTarget()->getID());

        // Is the connection compatible?
        if(connection->isCompatible())
        {
            // remove the error code if this connection has been incorrect before
            if(errors[Type::connection].contains(code))
               errors[Type::connection].removeOne(code);
        }
        else
        {
            // add the error code if this connection has been correct before
            if(!errors[Type::connection].contains(code))
                errors[Type::connection].append(code);
        }
    }
    // Is the parent a subsystem?
    else if(subsystem)
    {
        // generate error code characterising the connection
        QString code = QString("SUB %1: CON: %2,%3;%4,%5")
                .arg(subsystem->getID())
                .arg(connection->getSource()->getComponent()->getID())
                .arg(connection->getSource()->getID())
                .arg(connection->getTarget()->getComponent()->getID())
                .arg(connection->getTarget()->getID());

        // Is the connection compatible?
        if(connection->isCompatible())
        {
            // remove the error code if this connection has been incorrect before
            if(errors[Type::connection].contains(code))
                errors[Type::connection].removeOne(code);
        }
        else
        {
            // add the error code if this connection has been correct before
            if(!errors[Type::connection].contains(code))
                errors[Type::connection].append(code);
        }
    }
}

void SystemErrorModel::checkInputConnection(const SubsystemInputConnectionItemInterface * const connection)
{
    // Access subsystem in which input connection exists
    SubsystemItemInterface * subsystem = qobject_cast<SubsystemItemInterface *>(connection->parent()->parent());

    if(subsystem)
    {
        // generate error code characterising the input connection
        QString code = QString("SUB %1: INCON: %2;%3,%4")
                .arg(subsystem->getID())
                .arg(connection->getSource()->getID())
                .arg(connection->getTarget()->getComponent()->getID())
                .arg(connection->getTarget()->getID());

        // Is the connection compatible?
        if(connection->isCompatible())
        {
            // remove the error code if this connection has been incorrect before
            if(errors[Type::connection].contains(code))
               errors[Type::connection].removeOne(code);
        }
        else
        {
            // add the error code if this connection has been correct before
            if(!errors[Type::connection].contains(code))
                errors[Type::connection].append(code);
        }
    }
}

void SystemErrorModel::checkOutputConnection(const SubsystemOutputConnectionItemInterface * const connection)
{
    // Access subsystem in which input connection exists
    SubsystemItemInterface * subsystem = qobject_cast<SubsystemItemInterface *>(connection->parent()->parent());

    if(subsystem)
    {
        // generate error code characterising the output connection
        QString code = QString("SUB %1: OUTCON: %2,%3;%4")
                .arg(subsystem->getID())
                .arg(connection->getSource()->getComponent()->getID())
                .arg(connection->getSource()->getID())
                .arg(connection->getTarget()->getID());

        // Is the connection compatible?
        if(connection->isCompatible())
        {
            // remove the error code if this connection has been incorrect before
            if(errors[Type::connection].contains(code))
               errors[Type::connection].removeOne(code);
        }
        else
        {
            // add the error code if this connection has been correct before
            if(!errors[Type::connection].contains(code))
                errors[Type::connection].append(code);
        }
    }
}

void SystemErrorModel::checkComponentInputs(const SystemComponentItemInterface * const systemComponent)
{
    // access system or subsystem
    SystemItemInterface * systemitem = qobject_cast<SystemItemInterface*>(systemComponent->parent()->parent());
    SubsystemItemInterface * subsystemitem = qobject_cast<SubsystemItemInterface*>(systemComponent->parent()->parent());

    // create list of input IDs involved in the component
    QList<SystemComponentInputItemInterface::ID> unconnected={};
    for(SystemComponentInputItemInterface* input : *(systemComponent->getInputs()) )
        unconnected.append(input->getID());

    // If the system component is in a system...
    if(systemitem)
    {
        // go through all connections and check which inputs are involved in connections
        for(auto connection : systemitem->getConnections()->values())
        {
            SystemComponentInputItemInterface::ID input_id = connection->getTarget()->getID();
            SystemComponentItemInterface::ID component_id = connection->getTarget()->getComponent()->getID();

            if( unconnected.contains(input_id) && component_id == systemComponent->getID())
                unconnected.removeOne(input_id);
        }

        // generate error code for all unconnected inputs
        for(auto id : unconnected)
        {
            QString code = QString("SYS %1: IN: %2,%3").arg(systemitem->getID()).arg(systemComponent->getID()).arg(id);

            if(!errors[Type::input].contains(code))
                errors[Type::input].append(code);

        }

    }
    // If the system component is in a subsystem...
    else if(subsystemitem)
    {
        // Check whether inputs are involved in connections
        for(auto connection : subsystemitem->getConnections()->values())
        {
            SystemComponentInputItemInterface::ID input_id = connection->getTarget()->getID();
            SystemComponentItemInterface::ID component_id = connection->getTarget()->getComponent()->getID();

            if( unconnected.contains(input_id) && component_id==systemComponent->getID())
                unconnected.removeOne(input_id);
        }

        // check whether inputs are involved in input connections
        for(auto connection : subsystemitem->getInputConnections()->values())
        {
            SystemComponentInputItemInterface::ID input_id = connection->getTarget()->getID();
            SystemComponentItemInterface::ID component_id = connection->getTarget()->getComponent()->getID();

            if( unconnected.contains(input_id) && component_id==systemComponent->getID())
                unconnected.removeOne(input_id);
        }

        // generate error code for unconnected inputs
        for(auto id : unconnected)
        {
            QString code = QString("SUB %1: IN: %2,%3").arg(subsystemitem->getID()).arg(systemComponent->getID()).arg(id);
            errors[Type::input].append(code);
        }
    }
}

void SystemErrorModel::checkSystemItem(const SystemItemInterface * const system)
{
    // Check for incompatible connections
    for(auto connection : system->getConnections()->values())
        checkConnection(connection);

    // Check for unused component inputs
    for(auto component : system->getComponents()->values())
        checkComponentInputs(component);
}

void SystemErrorModel::checkSubsystemItem(const SubsystemItemInterface * const subsystem)
{
    // Check for incompatible connections
    for(auto connection : subsystem->getConnections()->values())
        checkConnection(connection);

    // Check for incompatible input connections
    for(auto connection : subsystem->getInputConnections()->values())
        checkInputConnection(connection);

    // Check for incompatible input connections
    for(auto connection : subsystem->getOutputConnections()->values())
        checkOutputConnection(connection);

    // Check for unused component inputs
    for(auto component : subsystem->getComponents()->values())
        checkComponentInputs(component);
}

void SystemErrorModel::checkSystem()
{
    clearErrors();

    // Check system items
    for(auto item : system->getSystems()->values())
        checkSystemItem(item);


    // Check subsystem items
    for(auto item : system->getSubsystems()->values())
        checkSubsystemItem(item);


}

