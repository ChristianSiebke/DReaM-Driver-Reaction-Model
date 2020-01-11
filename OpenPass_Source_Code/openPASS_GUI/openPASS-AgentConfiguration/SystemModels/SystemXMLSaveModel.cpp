#include "SystemXMLSaveModel.h"

#include <QFile>
#include <QIODevice>
#include <QXmlStreamWriter>
#include "openPASS-System/SystemComponentMapInterface.h"
#include "openPASS-System/SystemComponentParameterMapInterface.h"
#include "openPASS-System/SystemComponentScheduleInterface.h"
#include "openPASS-System/SystemConnectionMapInterface.h"
#include "openPASS-System/SystemItemInterface.h"
#include "openPASS-System/SystemMapInterface.h"

bool SystemXMLSaveModel::save(QString const & filepath,
                              SystemMapInterface const * const systems,
                              SubsystemMapInterface const * const subsystems)
{
    // Can we open the file in write only mode?
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Save system to file
        return save(&file, systems,subsystems);
    }
    // Return failure
    return false;
}

bool SystemXMLSaveModel::save(QIODevice * const device,
                              SystemMapInterface const * const systems,
                              SubsystemMapInterface const * const subsystems)
{
    // Initialize xml stream writer
    QXmlStreamWriter xml(device);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);

    // Save system to xml stream
    xml.writeStartDocument();

    xml.writeStartElement(KeySystems);


    for (SystemItemInterface const * const system : *systems)
    {
        saveSystem(xml, system);
    }
    for(SubsystemItemInterface const * const subsystem : *subsystems)
    {
        saveSubsystem(xml,subsystem);
    }

    xml.writeEndElement();
    xml.writeEndDocument();
    return true;
}

void SystemXMLSaveModel::saveSystem(QXmlStreamWriter & xml,
                                    SystemItemInterface const * const system)
{
    xml.writeStartElement(KeySystem);
    xml.writeTextElement(KeySystemID, QString::number(system->getID()));
    xml.writeTextElement(KeySystemTitle, system->getTitle());
    xml.writeTextElement(KeySystemPriority, QString::number(system->getPriority()));
    saveSystemComponent(xml, system->getComponents());
    saveSystemConnection(xml, system->getConnections());
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSystemComponent(QXmlStreamWriter &xml,
                                             SystemComponentMapInterface const * const components)
{
    xml.writeStartElement(KeySystemComponents);
    for (SystemComponentMapInterface::Item const * const component: * components)
    {
        xml.writeStartElement(KeySystemComponent);
        xml.writeTextElement(KeySystemComponentID, QString::number(component->getID()));
        xml.writeTextElement(KeySystemComponentLibrary, component->getLibrary());
        xml.writeTextElement(KeySystemComponentTitle, component->getTitle());
        saveSystemComponentSchedule(xml, component->getSchedule());
        saveSystemComponentParameters(xml, component->getParameters());
        xml.writeStartElement(KeySystemComponentPosition);
        xml.writeTextElement(KeySystemComponentPositionX, QString::number(component->getPosition().x()));
        xml.writeTextElement(KeySystemComponentPositionY, QString::number(component->getPosition().y()));
        xml.writeEndElement();
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSystemComponentSchedule(QXmlStreamWriter &xml,
                                                     SystemComponentScheduleInterface const * const schedule)
{
    xml.writeStartElement(KeySystemComponentSchedule);
    xml.writeTextElement(KeySystemComponentSchedulePriority, QString::number(schedule->getPriority()));
    xml.writeTextElement(KeySystemComponentScheduleOffset, QString::number(schedule->getOffset()));
    xml.writeTextElement(KeySystemComponentScheduleCycle, QString::number(schedule->getCycle()));
    xml.writeTextElement(KeySystemComponentScheduleResponse, QString::number(schedule->getResponse()));
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSystemComponentParameters(QXmlStreamWriter &xml,
                                                       SystemComponentParameterMapInterface const * const parameters)
{
    if (!(parameters->count() > 0))
    {
        xml.writeEmptyElement(KeySystemComponentParameters);
        return;
    }
    xml.writeStartElement(KeySystemComponentParameters);
    for (SystemComponentParameterMapInterface::Item const * const parameter : *parameters)
    {
        xml.writeStartElement(KeySystemComponentParameter);
        xml.writeTextElement(KeySystemComponentParameterID, QString::number(parameter->getID()));
        xml.writeTextElement(KeySystemComponentParameterType, parameter->getType());
        xml.writeTextElement(KeySystemComponentParameterUnit, parameter->getUnit());
        xml.writeTextElement(KeySystemComponentParameterValue, parameter->getValue());
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSystemConnection(QXmlStreamWriter &xml,
                                              SystemConnectionMapInterface const * const connections)
{
    xml.writeStartElement(KeySystemConnections);
    for (SystemConnectionMapInterface::Item const * const connection: *connections)
    {
        xml.writeStartElement(KeySystemConnection);
        xml.writeTextElement(KeySystemConnectionID, QString::number(connection->getID()));
        xml.writeStartElement(KeySystemConnectionSource);
        xml.writeTextElement(KeySystemConnectionSourceComponent, QString::number(
                                 connection->getSource()->getComponent()->getID()));
        xml.writeTextElement(KeySystemConnectionSourceOutput, QString::number(
                                 connection->getSource()->getID()));
        xml.writeEndElement();
        xml.writeStartElement(KeySystemConnectionTarget);
        xml.writeTextElement(KeySystemConnectionTargetComponent, QString::number(
                                 connection->getTarget()->getComponent()->getID()));
        xml.writeTextElement(KeySystemConnectionTargetInput, QString::number(
                                 connection->getTarget()->getID()));
        xml.writeEndElement();
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSubsystem(QXmlStreamWriter &xml,
                                       const SubsystemItemInterface * const subsystem)
{
    xml.writeStartElement(KeySubsystem);
    xml.writeTextElement(KeySubsystemID, QString::number(subsystem->getID()));
    xml.writeTextElement(KeySubsystemTitle, subsystem->getTitle());

    saveSubsystemInputs(xml,subsystem->getInputs());
    saveSubsystemOutputs(xml,subsystem->getOutputs());

    saveSystemComponent(xml,subsystem->getComponents());

    saveSubsystemParameters(xml,subsystem->getParameters());

    saveSystemConnection(xml,subsystem->getConnections());
    saveSubsystemInputConnection(xml,subsystem->getInputConnections());
    saveSubsystemOutputConnection(xml,subsystem->getOutputConnections());
    xml.writeEndElement();

}

void SystemXMLSaveModel::saveSubsystemParameters(QXmlStreamWriter & xml,
                                        SubsystemParameterMapInterface const * const parameters)
{
    xml.writeStartElement(KeySubsystemParameters);
    for(SubsystemParameterMapInterface::Item const * const parameter: *parameters)
    {
        xml.writeStartElement(KeySubsystemParameter);
        xml.writeTextElement(KeySubsystemParameterID, QString::number(parameter->getID()));
        xml.writeTextElement(KeySubsystemParameterTitle, parameter->getTitle());
        xml.writeTextElement(KeySubsystemParameterType, parameter->getType());
        xml.writeTextElement(KeySubsystemParameterUnit, parameter->getUnit());
        xml.writeTextElement(KeySubsystemParameterValue, parameter->getValue());
        xml.writeStartElement(KeySubsystemParameterSource);
        xml.writeTextElement(KeySubsystemParameterSourceComponent, QString::number(parameter->getSystemComponentParameter()->getComponent()->getID()));
        xml.writeTextElement(KeySubsystemParameterSourceParameter, QString::number(parameter->getSystemComponentParameter()->getID()));
        xml.writeEndElement();
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSubsystemInputs(QXmlStreamWriter & xml,
                                        SubsystemInputMapInterface const * const inputs)
{
    xml.writeStartElement(KeySubsystemInputs);
    for(SubsystemInputMapInterface::Item const * const input: *inputs)
    {
        xml.writeStartElement(KeySubsystemInput);
        xml.writeTextElement(KeySubsystemInputID, QString::number(input->getID()));
        xml.writeTextElement(KeySubsystemInputTitle, input->getTitle());
        xml.writeTextElement(KeySubsystemInputType, input->getType());
        xml.writeTextElement(KeySubsystemInputUnit, input->getUnit());
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSubsystemOutputs(QXmlStreamWriter & xml,
                                        SubsystemOutputMapInterface const * const outputs)
{
    xml.writeStartElement(KeySubsystemOutputs);
    for(SubsystemOutputMapInterface::Item const * const output: *outputs)
    {
        xml.writeStartElement(KeySubsystemOutput);
        xml.writeTextElement(KeySubsystemOutputID, QString::number(output->getID()));
        xml.writeTextElement(KeySubsystemOutputTitle, output->getTitle());
        xml.writeTextElement(KeySubsystemOutputType, output->getType());
        xml.writeTextElement(KeySubsystemOutputUnit, output->getUnit());
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSubsystemInputConnection(QXmlStreamWriter &xml,
                                              SubsystemInputConnectionMapInterface const * const inputconnections)
{
    xml.writeStartElement(KeySubsystemInputConnections);
    for (SubsystemInputConnectionMapInterface::Item const * const inputconnection: *inputconnections)
    {
        xml.writeStartElement(KeySubsystemInputConnection);
        xml.writeTextElement(KeySubsystemInputConnectionID, QString::number(inputconnection->getID()));
        xml.writeTextElement(KeySubsystemInputConnectionSource, QString::number(inputconnection->getSource()->getID()));

        xml.writeStartElement(KeySubsystemInputConnectionTarget);
        xml.writeTextElement(KeySubsystemInputConnectionTargetComponent, QString::number(
                                 inputconnection->getTarget()->getComponent()->getID()));
        xml.writeTextElement(KeySubsystemInputConnectionTargetInput, QString::number(
                                 inputconnection->getTarget()->getID()));
        xml.writeEndElement();
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void SystemXMLSaveModel::saveSubsystemOutputConnection(QXmlStreamWriter &xml,
                                              SubsystemOutputConnectionMapInterface const * const outputconnections)
{
    xml.writeStartElement(KeySubsystemOutputConnections);
    for (SubsystemOutputConnectionMapInterface::Item const * const outputconnection: *outputconnections)
    {
        xml.writeStartElement(KeySubsystemOutputConnection);
        xml.writeTextElement(KeySubsystemOutputConnectionID, QString::number(outputconnection->getID()));

        xml.writeStartElement(KeySubsystemOutputConnectionSource);
        xml.writeTextElement(KeySubsystemOutputConnectionSourceComponent, QString::number(
                                 outputconnection->getSource()->getComponent()->getID()));
        xml.writeTextElement(KeySubsystemOutputConnectionSourceOutput, QString::number(
                                 outputconnection->getSource()->getID()));
        xml.writeEndElement();

        xml.writeTextElement(KeySubsystemOutputConnectionTarget, QString::number(outputconnection->getTarget()->getID()));
        xml.writeEndElement();
    }
    xml.writeEndElement();
}
