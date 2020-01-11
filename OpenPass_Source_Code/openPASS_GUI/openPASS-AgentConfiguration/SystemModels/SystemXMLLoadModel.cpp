#include "SystemXMLLoadModel.h"

#include "SystemComponentItemModel.h"
#include "SystemItemModel.h"
#include "SubsystemItemModel.h"
#include <QIODevice>
#include <QFile>
#include <QXmlStreamReader>

bool SystemXMLLoadModel::load(QString const & filepath,
                              SystemMapInterface * const systems,
                              SubsystemMapInterface * const subsystems,
                              SystemComponentManagerInterface const * const manager)
{
    // Does the file exist and can we open it?
    QFile file(filepath);

    if ((file.exists()) && (file.open(QIODevice::ReadOnly | QIODevice::Text)))
    {
        // Load component from file
        return load(&file, systems, subsystems,manager);
    }
    // Return failure
    return false;
}


bool SystemXMLLoadModel::load(QIODevice * const device,
                              SystemMapInterface * const systems,
                              SubsystemMapInterface * const subsystems,
                              SystemComponentManagerInterface const * const manager)
{
    // Initialize xml stream readers
    // One reader is needed for loading the xml, the other is needed for a pre-scan of
    // subsystems
    QXmlStreamReader xml(device);
    QXmlStreamReader xml_subsystems(device);

    bool success=true;

  //  systems->clear();
  //  subsystems->clear();

    // do the pre-scan of subsystems
    success = success && registerSubsystems(xml_subsystems, manager);

    // reset the device
    device->close();
    device->open(QIODevice::ReadOnly | QIODevice::Text);

    // Some XML verification needed?
    xml.readNextStartElement();

    // read root element
    if(xml.name() == KeySystems)
    {
        while(xml.readNextStartElement())
        {
            if(xml.name() == KeySubsystem)
                success = loadSubsystemItem(xml,subsystems,manager) && success;
            else if(xml.name() == KeySystem)
                success = loadSystemItem(xml,systems,manager) && success;
            else
                xml.skipCurrentElement();
        }
        return success;
    }
    else
        return false;
}


bool SystemXMLLoadModel::registerSubsystems(QXmlStreamReader & xml,
                        SystemComponentManagerInterface const * const manager)
{

    bool success = true;

    // to skip the root element
    xml.readNextStartElement();

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystem)
            success=registerSubsystemItem(xml, manager) && success;

        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::registerSubsystemItem(QXmlStreamReader & xml,
                                               SystemComponentManagerInterface const * const manager)
{
    ComponentItemInterface * component;
    QString name;
    bool success = true;

    // check that it is the ID
    xml.readNextStartElement();
    if(xml.name() == KeySubsystemID)
        xml.skipCurrentElement();
    else
        return false;

    // check that it is the title, if so, create the component and add it under this
    // name to the manager
    xml.readNextStartElement();
    if(xml.name() == KeySubsystemTitle)
    {
        name = xml.readElementText();
        manager->getComponents()->add(name);
        component = manager->getComponents()->getItem(name);
        component->setTitle(name);
        component->setType(static_cast<ComponentMapInterface::Item::Type>(5)); // type 5 = Subsystem

        // put some default schedule
        component->getSchedule()->setOffset(0);
        component->getSchedule()->setCycle(1);
        component->getSchedule()->setResponse(0);

    }
    else
        return false;

    // Read remaining relevant ingredients of the Subsystem component
    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemParameters)
            success = registerSubsystemParameters(xml,component) && success;

        else if(xml.name() == KeySubsystemInputs)
            success = registerSubsystemInputs(xml, component) && success;

        else if (xml.name() == KeySubsystemOutputs)
            success = registerSubsystemOutputs(xml, component) && success;

        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::registerSubsystemParameters(QXmlStreamReader & xml, ComponentItemInterface * component)
{
    ComponentParameterItemInterface::ID id;
    QList<QString> keys;
    bool success=true;

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemParameter)
        {
            // create list of necessary items
            keys = KeyListSubsystemParameter;

            // ignore source, as this is not relevant for creation of component
            keys.removeAll(KeySubsystemParameterSource);

            // verify ID and register parameter
            xml.readNextStartElement();
            if(xml.name() == KeySubsystemParameterID)
            {
                id = xml.readElementText().toInt();
                component->getParameters()->add(id);
                keys.removeAll(KeySubsystemParameterID);
            }
            else
                return false;

            while(xml.readNextStartElement())
            {
                if(xml.name() == KeySubsystemParameterTitle)
                {
                    component->getParameters()
                            ->getItem(id)->setTitle(xml.readElementText());

                    keys.removeAll(KeySubsystemParameterTitle);

                }
                else if(xml.name() == KeySubsystemParameterType)
                {
                    component->getParameters()
                            ->getItem(id)->setType(xml.readElementText());

                    keys.removeAll(KeySubsystemParameterType);

                }
                else if(xml.name() == KeySubsystemParameterUnit)
                {
                    component->getParameters()
                            ->getItem(id)->setUnit(xml.readElementText());

                    keys.removeAll(KeySubsystemParameterUnit);

                }
                else if(xml.name() == KeySubsystemParameterValue)
                {
                    component->getParameters()
                            ->getItem(id)->setValue(xml.readElementText());
                    keys.removeAll(KeySubsystemParameterValue);

                }
                else
                    xml.skipCurrentElement();
            }

            success = success && keys.isEmpty();

        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::registerSubsystemInputs(QXmlStreamReader & xml,
                                                 ComponentItemInterface * component)
{
    ComponentInputItemInterface::ID id;
    bool success = true;
    QList<QString> keys;

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemInput)
        {
            // create list of necessary input ingredients
            keys = KeyListSubsystemInput;

            // verify first element (=ID) and add input
            xml.readNextStartElement();
            if(xml.name()==KeySubsystemInputID)
            {
                id = xml.readElementText().toInt();
                component->getInputs()->add(id);
                keys.removeAll(KeySubsystemInputID);
            }
            else
                return false;

            // register remaining ingredients
            while(xml.readNextStartElement())
            {
                if(xml.name() == KeySubsystemInputTitle)
                {
                    component->getInputs()
                            ->getItem(id)->setTitle(xml.readElementText());

                    keys.removeAll(KeySubsystemInputTitle);

                }
                else if(xml.name() == KeySubsystemInputType)
                {
                    component->getInputs()
                            ->getItem(id)->setType(xml.readElementText());

                    keys.removeAll(KeySubsystemInputType);

                }
                else if(xml.name() == KeySubsystemInputUnit)
                {
                    component->getInputs()
                            ->getItem(id)->setUnit(xml.readElementText());

                    keys.removeAll(KeySubsystemInputUnit);

                }
                else
                    xml.skipCurrentElement();
            }

            success == success && keys.isEmpty();

        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::registerSubsystemOutputs(QXmlStreamReader & xml,
                                                 ComponentItemInterface * component)
{
    ComponentInputItemInterface::ID id;
    bool success = true;
    QList<QString> keys;

    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemOutput)
        {
            // create list of necessary input ingredients
            keys = KeyListSubsystemOutput;

            // verify first element (=ID) and add output
            xml.readNextStartElement();
            if(xml.name()==KeySubsystemOutputID)
            {
                id = xml.readElementText().toInt();
                component->getOutputs()->add(id);
                keys.removeAll(KeySubsystemOutputID);
            }
            else
                return false;

            // register remaining ingredients
            while(xml.readNextStartElement())
            {
                if(xml.name() == KeySubsystemOutputTitle)
                {
                    component->getOutputs()
                            ->getItem(id)->setTitle(xml.readElementText());

                    keys.removeAll(KeySubsystemOutputTitle);

                }
                else if(xml.name() == KeySubsystemOutputType)
                {
                    component->getOutputs()
                            ->getItem(id)->setType(xml.readElementText());

                    keys.removeAll(KeySubsystemOutputType);

                }
                else if(xml.name() == KeySubsystemOutputUnit)
                {
                    component->getOutputs()
                            ->getItem(id)->setUnit(xml.readElementText());

                    keys.removeAll(KeySubsystemOutputUnit);

                }
                else
                    xml.skipCurrentElement();
            }

            success == success && keys.isEmpty();

        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::loadSystemItem(QXmlStreamReader & xml,
                                        SystemMapInterface * const systems,
                                        SystemComponentManagerInterface const * const manager)
{
    bool success = true;
    QList<QString> keys = KeyListSystem;

    keys.removeAll(xml.name().toString());

    // read ID and add system to the map
    xml.readNextStartElement();
    SystemMapInterface::ID const id = xml.readElementText().toInt();
    systems->add(id);
    SystemMapInterface::Item *system = systems->getItem(id);//new SystemItemModel( (SystemComponentManagerInterface * const) (manager) );

    while (xml.readNextStartElement())
    {

        if (xml.name() == KeySystemTitle)
        {
            system->setTitle(xml.readElementText());
            Q_EMIT systems->modifiedTitle(system->getID());
        }
        else if (xml.name() == KeySystemPriority)
            system->setPriority(xml.readElementText().toUInt());
        else if (xml.name() == KeySystemComponents)
        {
            // Enable component modify mode
            Q_EMIT system->getComponents()->mode(true);
            // Load components from xml stream
            success = loadSystemComponentMap(
                        xml, system->getComponents(), manager) && success;
            // Disable component modify mode
            Q_EMIT system->getComponents()->mode(false);
        }
        else if (xml.name() == KeySystemConnections)
        {
            // Enable connection modify mode
            Q_EMIT system->getConnections()->mode(true);
            // Load connections from xml stream
            success = loadSystemConnectionMap(
                        xml, system->getComponents(), system->getConnections()) && success;
            // Disable connection modify mode
            Q_EMIT system->getConnections()->mode(false);
        }
        else
            xml.skipCurrentElement();
    }

    return success && keys.isEmpty();
}

bool SystemXMLLoadModel::loadSubsystemItem(QXmlStreamReader & xml,
                                        SubsystemMapInterface * const subsystems,
                                        SystemComponentManagerInterface const * const manager)
{
    bool success = true;
    QList<QString> keys = KeyListSubsystem;

    keys.removeAll(xml.name().toString());

    // Read ID and Title, create subsystem and add it to the component map
    xml.readNextStartElement();
    SubsystemMapInterface::ID id = xml.readElementText().toInt();
    xml.readNextStartElement();
    SubsystemMapInterface::Item::Title name = xml.readElementText();
    subsystems->add(id,manager->getComponents()->getItem(name));
    SubsystemMapInterface::Item *subsystem = subsystems->getItem(id);//new SubsystemItemModel((SystemComponentManagerInterface * const) (manager));

    while (xml.readNextStartElement())
    {

        if (xml.name() == KeySubsystemInputs)
        {
            // Enable inputs modify mode
            Q_EMIT subsystem->getInputs()->mode(true);

            // Load subsystem inputs from xml stream
            success = loadSubsystemInputMap( xml,subsystem->getInputs() ) && success;

            // Disable inputs modify mode
            Q_EMIT subsystem->getInputs()->mode(false);
        }
        else if (xml.name() == KeySubsystemOutputs)
        {
            // Enable outputs modify mode
            Q_EMIT subsystem->getOutputs()->mode(true);

            // Load subsystem outputs from xml stream
            success = loadSubsystemOutputMap( xml,subsystem->getOutputs() ) && success;

            // Disable outputs modify mode
            Q_EMIT subsystem->getOutputs()->mode(false);
        }

        else if (xml.name() == KeySystemComponents)
        {
            // Enable component modify mode
            Q_EMIT subsystem->getComponents()->mode(true);
            // Load components from xml stream
            success = loadSystemComponentMap(
                        xml, subsystem->getComponents(), manager) && success;
            // Disable component modify mode
            Q_EMIT subsystem->getComponents()->mode(false);
        }

        else if (xml.name() == KeySubsystemParameters)
            success = loadSubsystemParameterMap( xml,manager->getComponents()->getItem(name)->getParameters(),
                                                 subsystem->getParameters(),subsystem->getComponents()) && success;

        else if (xml.name() == KeySystemConnections)
        {
            // Enable connection modify mode
            Q_EMIT subsystem->getConnections()->mode(true);
            // Load connections from xml stream
            success = loadSystemConnectionMap(
                        xml, subsystem->getComponents(), subsystem->getConnections()) && success;
            // Disable connection modify mode
            Q_EMIT subsystem->getConnections()->mode(false);
        }
        else if (xml.name() == KeySubsystemInputConnections)
        {
            // Enable connection modify mode
            Q_EMIT subsystem->getInputConnections()->mode(true);
            // Load input connections from xml stream
            success = loadSubsystemInputConnectionMap(
                        xml, subsystem->getComponents(), subsystem->getInputs(),
                        subsystem->getInputConnections()) && success;
            // Disable connection modify mode
            Q_EMIT subsystem->getInputConnections()->mode(false);
        }
        else if (xml.name() == KeySubsystemOutputConnections)
        {
            // Enable connection modify mode
            Q_EMIT subsystem->getOutputConnections()->mode(true);
            // Load output connections from xml stream
            success = loadSubsystemOutputConnectionMap(
                        xml, subsystem->getComponents(), subsystem->getOutputs(),
                        subsystem->getOutputConnections()) && success;
            // Disable connection modify mode
            Q_EMIT subsystem->getOutputConnections()->mode(false);
        }
        else
            xml.skipCurrentElement();
    }

    return success && keys.isEmpty();
}

bool SystemXMLLoadModel::loadSystemComponentMap(QXmlStreamReader & xml,
                                                SystemComponentMapInterface * const components,
                                                SystemComponentManagerInterface const * const manager)
{
    bool success = true;
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeySystemComponent)
            success = loadSystemComponentItem(xml, components, manager) && success;
        else
            xml.skipCurrentElement();
    }
    return success;
}

bool SystemXMLLoadModel::loadSystemComponentItem(QXmlStreamReader & xml,
                                                 SystemComponentMapInterface * const components,
                                                 SystemComponentManagerInterface const * const manager)
{
    bool success = true;
    QList<QString> keys = KeyListSystemComponent;
    SystemComponentMapInterface::Item * component;

    // Load ID from xml stream
    xml.readNext();
    if (!((xml.readNext() == QXmlStreamReader::TokenType::StartElement) &&
          (xml.name() == KeySystemComponentID)))
        return false;
    SystemComponentMapInterface::ID const id = xml.readElementText().toUInt();
    keys.removeAll(xml.name().toString());
    // Load library from xml stream, lookup component and create system component
    xml.readNext();
    if (!((xml.readNext() == QXmlStreamReader::TokenType::StartElement) &&
          (xml.name() == KeySystemComponentLibrary)))
        return false;

    //
    QString library = xml.readElementText();

    // is component defined in a XML?
    ComponentItemInterface const * const base = manager->lookupItemByName(library);
    if (base)
    {
       component = new SystemComponentItemModel(base,QPoint(0,0),components);
    }
    else
        return false;

    keys.removeAll(xml.name().toString());

    // Load system component from xml stream
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemComponentTitle)
            component->setTitle(xml.readElementText());
        else if (xml.name() == KeySystemComponentPriority)
            component->getSchedule()->setPriority(xml.readElementText().toUInt());
        else if (xml.name() == KeySystemComponentSchedule)
            success = loadSystemComponentSchedule(xml, component->getSchedule()) && success;
        else if (xml.name() == KeySystemComponentParameters)
            success = loadSystemComponentParameterMap(xml, component->getParameters()) && success;
        else if (xml.name() == KeySystemComponentPosition)
            success = loadSystemComponentPosition(xml, component) && success;
        else
            xml.skipCurrentElement();
    }
    // Add system component to system
    if (success && keys.isEmpty() && components->add(id, component))
        return true;
    delete component;
    return false;
}

bool SystemXMLLoadModel::loadSystemComponentSchedule(QXmlStreamReader & xml,
                                                     SystemComponentScheduleInterface * const schedule)
{
    QList<QString> keys = KeyListSystemComponentSchedule;
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemComponentSchedulePriority)
            schedule->setPriority(xml.readElementText().toUInt());
        else if (xml.name() == KeySystemComponentScheduleOffset)
            schedule->setOffset(xml.readElementText().toUInt());
        else if (xml.name() == KeySystemComponentScheduleCycle)
            schedule->setCycle(xml.readElementText().toUInt());
        else if (xml.name() == KeySystemComponentScheduleResponse)
            schedule->setResponse(xml.readElementText().toUInt());
        else
            xml.skipCurrentElement();
    }
    return keys.isEmpty();
}

bool SystemXMLLoadModel::loadSubsystemParameterMap(QXmlStreamReader & xml,
                                                   ComponentParameterMapInterface * const componentParameters,
                                                   SubsystemParameterMapInterface * const parameters,
                                                   SystemComponentMapInterface * const components)
{
    bool success=true;
    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemParameter)
        {
            success = loadSubsystemParameterItem(xml, componentParameters, parameters, components) && success;
        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::loadSubsystemParameterItem(QXmlStreamReader &xml,
                                                    ComponentParameterMapInterface * const componentParameters,
                                                    SubsystemParameterMapInterface * const parameters,
                                                    SystemComponentMapInterface * const components)
{
    QList<QString> keys = KeyListSubsystemParameter;

    SubsystemParameterMapInterface::ID id = 0;
    SubsystemParameterItemInterface::Title title = SubsystemParameterItemInterface::Title();
    SubsystemParameterItemInterface::Type type = SubsystemParameterItemInterface::Type();
    SubsystemParameterItemInterface::Unit unit = SubsystemParameterItemInterface::Unit();
    SubsystemParameterItemInterface::Value value = SubsystemParameterItemInterface::Value();

    QList<QString> keys_source =KeyListSubsystemParameterSource;
    SystemComponentParameterItemInterface::ID id_component;
    SystemComponentParameterItemInterface::ID id_parameter;

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySubsystemParameterID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySubsystemParameterTitle)
            title = xml.readElementText();
        else if (xml.name() == KeySubsystemParameterType)
            type = xml.readElementText();
        else if (xml.name() == KeySubsystemParameterUnit)
            unit = xml.readElementText();
        else if (xml.name() == KeySubsystemParameterValue)
            value = xml.readElementText();
        else if (xml.name() == KeySubsystemParameterSource)
        {
            while(xml.readNextStartElement())
            {
                keys_source.removeAll(xml.name().toString());
                if(xml.name() == KeySubsystemParameterSourceComponent)
                    id_component = xml.readElementText().toUInt();
                else if (xml.name() == KeySubsystemParameterSourceParameter)
                    id_parameter = xml.readElementText().toUInt();
                else
                    xml.skipCurrentElement();
            }
        }
        else
            xml.skipCurrentElement();
    }

    if (keys.isEmpty() && keys_source.isEmpty())
    {
        parameters->add(componentParameters->getItem(id), components->getItem(id_component)
                        ->getParameters()->getItem(id_parameter));

        parameters->getItem(id)->setTitle(title);
        parameters->getItem(id)->setValue(value);
        return true;
    }
    else
        return false;

}

bool SystemXMLLoadModel::loadSubsystemInputMap(QXmlStreamReader & xml,
                                                   SubsystemInputMapInterface * const inputs)
{
    bool success=true;
    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemInput)
        {
            success = loadSubsystemInputItem(xml, inputs) && success;
        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::loadSubsystemInputItem(QXmlStreamReader &xml,
                                                    SubsystemInputMapInterface * const inputs)
{
    QList<QString> keys = KeyListSubsystemInput;
    SubsystemInputMapInterface::ID id = 0;
    SubsystemInputItemInterface::Title title = SubsystemInputItemInterface::Title();
    SubsystemInputItemInterface::Type type = SubsystemInputItemInterface::Type();
    SubsystemInputItemInterface::Unit unit = SubsystemInputItemInterface::Unit();

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySubsystemInputID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySubsystemInputTitle)
            title = xml.readElementText();
        else if (xml.name() == KeySubsystemInputType)
            type = xml.readElementText();
        else if (xml.name() == KeySubsystemInputUnit)
            unit = xml.readElementText();
        else
            xml.skipCurrentElement();
    }
    if (keys.isEmpty())
    {
        inputs->add(id);
        inputs->getItem(id)->setTitle(title);
        inputs->getItem(id)->setType(type);
        inputs->getItem(id)->setUnit(unit);
        return true;
    }
    return false;

}

bool SystemXMLLoadModel::loadSubsystemOutputMap(QXmlStreamReader & xml,
                                                   SubsystemOutputMapInterface * const outputs)
{
    bool success=true;
    while(xml.readNextStartElement())
    {
        if(xml.name() == KeySubsystemOutput)
        {
            success = loadSubsystemOutputItem(xml, outputs) && success;
        }
        else
            xml.skipCurrentElement();
    }

    return success;
}

bool SystemXMLLoadModel::loadSubsystemOutputItem(QXmlStreamReader &xml,
                                                    SubsystemOutputMapInterface * const outputs)
{
    QList<QString> keys = KeyListSubsystemOutput;
    SubsystemOutputMapInterface::ID id = 0;
    SubsystemOutputItemInterface::Title title = SubsystemOutputItemInterface::Title();
    SubsystemOutputItemInterface::Type type = SubsystemOutputItemInterface::Type();
    SubsystemOutputItemInterface::Unit unit = SubsystemOutputItemInterface::Unit();

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySubsystemOutputID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySubsystemOutputTitle)
            title = xml.readElementText();
        else if (xml.name() == KeySubsystemOutputType)
            type = xml.readElementText();
        else if (xml.name() == KeySubsystemOutputUnit)
            unit = xml.readElementText();
        else
            xml.skipCurrentElement();
    }
    if (keys.isEmpty())
    {
        outputs->add(id);
        outputs->getItem(id)->setTitle(title);
        outputs->getItem(id)->setType(type);
        outputs->getItem(id)->setUnit(unit);
        return true;
    }
    return false;

}

bool SystemXMLLoadModel::loadSystemComponentParameterMap(QXmlStreamReader & xml,
                                                         SystemComponentParameterMapInterface * const parameters)
{
    bool success = true;
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeySystemComponentParameter)
            success = loadSystemComponentParameterItem(xml, parameters) && success;
        else
            xml.skipCurrentElement();
    }
    return success;
}

bool SystemXMLLoadModel::loadSystemComponentParameterItem(QXmlStreamReader & xml,
                                                          SystemComponentParameterMapInterface * const parameters)
{
    QList<QString> keys = KeyListSystemComponentParameter;
    SystemComponentParameterMapInterface::ID id = 0;
    SystemComponentParameterItemInterface::Type type = SystemComponentParameterItemInterface::Type();
    SystemComponentParameterItemInterface::Unit unit = SystemComponentParameterItemInterface::Unit();
    SystemComponentParameterItemInterface::Value value = SystemComponentParameterItemInterface::Value();
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemComponentParameterID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySystemComponentParameterType)
            type = xml.readElementText();
        else if (xml.name() == KeySystemComponentParameterUnit)
            unit = xml.readElementText();
        else if (xml.name() == KeySystemComponentParameterValue)
            value = xml.readElementText();
        else
            xml.skipCurrentElement();
    }
    if (keys.isEmpty())
    {
        SystemComponentParameterItemInterface * const parameter = parameters->getItem(id);
        if ((parameter) && (parameter->getType() == type) && (parameter->getUnit() == unit))
        {
            parameter->setValue(value);
        }
        return true;
    }
    return false;
}

bool SystemXMLLoadModel::loadSystemComponentPosition(QXmlStreamReader & xml,
                                                     SystemComponentItemInterface * const component)
{
    QList<QString> keys = KeyListSystemComponentPosition;
    SystemComponentItemInterface::Position position = SystemComponentItemInterface::Position(0, 0);
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemComponentPositionX)
            position.setX(xml.readElementText().toInt());
        else if (xml.name() == KeySystemComponentPositionY)
            position.setY(xml.readElementText().toInt());
        else
            xml.skipCurrentElement();
    }
    return component->setPosition(position) && keys.isEmpty();
}

bool SystemXMLLoadModel::loadSystemConnectionMap(QXmlStreamReader & xml,
                                                 SystemComponentMapInterface const * const components,
                                                 SystemConnectionMapInterface * const connections)
{
    bool success = true;
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeySystemConnection)
            success = loadSystemConnectionItem(xml, components, connections) && success;
        else
            xml.skipCurrentElement();
    }
    return success;
}

bool SystemXMLLoadModel::loadSystemConnectionItem(QXmlStreamReader & xml,
                                                  SystemComponentMapInterface const * const components,
                                                  SystemConnectionMapInterface * const connections)
{
    bool success = true;
    QList<QString> keys = KeyListSystemConnection;
    SystemConnectionMapInterface::ID id = 0;
    SystemComponentMapInterface::ID sourceComponentID = 0;
    SystemComponentOutputMapInterface::ID sourceOutputID = 0;
    SystemComponentMapInterface::ID targetComponentID = 0;
    SystemComponentInputMapInterface::ID targetInputID = 0;
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemConnectionID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySystemConnectionSource)
            success = loadSystemConnectionItemSource(
                        xml, sourceComponentID, sourceOutputID) && success;
        else if (xml.name() == KeySystemConnectionTarget)
            success = loadSystemConnectionItemTarget(
                        xml, targetComponentID, targetInputID) && success;
        else
            xml.skipCurrentElement();
    }
    if (success && keys.isEmpty())
    {
        SystemComponentMapInterface::Item const * const sourceComponent =
                components->getItem(sourceComponentID);
        SystemComponentMapInterface::Item const * const targetComponent =
                components->getItem(targetComponentID);
        if (sourceComponent && targetComponent)
        {
            SystemConnectionMapInterface::Source const * const source =
                    sourceComponent->getOutputs()->getItem(sourceOutputID);
            SystemConnectionMapInterface::Target const * const target =
                    targetComponent->getInputs()->getItem(targetInputID);
            if (source && target)
            {
                connections->add(id, source, target);
            }
        }
        return true;
    }
    return false;
}

bool SystemXMLLoadModel::loadSystemConnectionItemSource(QXmlStreamReader & xml,
                                                        SystemComponentMapInterface::ID & sourceComponentID,
                                                        SystemComponentOutputMapInterface::ID & sourceOutputID)
{
    QList<QString> keys = KeyListSystemConnectionSource;
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemConnectionSourceComponent)
            sourceComponentID = xml.readElementText().toUInt();
        else if (xml.name() == KeySystemConnectionSourceOutput)
            sourceOutputID = xml.readElementText().toUInt();
        else
            xml.skipCurrentElement();
    }
    return keys.isEmpty();
}

bool SystemXMLLoadModel::loadSystemConnectionItemTarget(QXmlStreamReader & xml,
                                                        SystemComponentMapInterface::ID & targetComponentID,
                                                        SystemComponentInputMapInterface::ID & targetInputID)
{
    QList<QString> keys = KeyListSystemConnectionTarget;
    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySystemConnectionTargetComponent)
            targetComponentID = xml.readElementText().toUInt();
        else if (xml.name() == KeySystemConnectionTargetInput)
            targetInputID = xml.readElementText().toUInt();
        else
            xml.skipCurrentElement();
    }
    return keys.isEmpty();
}

bool SystemXMLLoadModel::loadSubsystemInputConnectionMap(QXmlStreamReader &xml,
                                     SystemComponentMapInterface const * const components,
                                     SubsystemInputMapInterface const * const inputs,
                                     SubsystemInputConnectionMapInterface * const inputconnections)
{
    bool success = true;
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeySubsystemInputConnection)
            success = loadSubsystemInputConnectionItem(xml, components, inputs, inputconnections) && success;
        else
            xml.skipCurrentElement();
    }
    return success;
}

bool SystemXMLLoadModel::loadSubsystemInputConnectionItem(QXmlStreamReader & xml,
                                                  SystemComponentMapInterface const * const components,
                                                  SubsystemInputMapInterface const * const inputs,
                                                  SubsystemInputConnectionMapInterface * const inputconnections)
{
    bool success = true;
    QList<QString> keys = KeyListSubsystemInputConnection;
    SubsystemInputConnectionMapInterface::ID id = 0;
    SystemComponentMapInterface::ID targetComponentID = 0;
    SystemComponentInputMapInterface::ID targetInputID = 0;
    SubsystemInputConnectionItemInterface::ID sourceInputID = 0;

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySubsystemInputConnectionID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySubsystemInputConnectionSource)
            sourceInputID = xml.readElementText().toUInt();
        else if (xml.name() == KeySubsystemInputConnectionTarget)
            success = loadSystemConnectionItemTarget(
                        xml, targetComponentID, targetInputID) && success;
        else
            xml.skipCurrentElement();
    }
    if (success && keys.isEmpty())
    {
        SubsystemInputMapInterface::Item const * const source =
                inputs->getItem(sourceInputID);
        SystemComponentMapInterface::Item const * const targetComponent =
                components->getItem(targetComponentID);
        if (source && targetComponent)
        {
            SystemConnectionMapInterface::Target const * const target =
                    targetComponent->getInputs()->getItem(targetInputID);
            if (source && target)
            {
                inputconnections->add(id, source, target);
            }
        }
        return true;
    }
    return false;
}

bool SystemXMLLoadModel::loadSubsystemOutputConnectionMap(QXmlStreamReader &xml,
                                     SystemComponentMapInterface const * const components,
                                     SubsystemOutputMapInterface const * const outputs,
                                     SubsystemOutputConnectionMapInterface * const outputconnections)
{
    bool success = true;
    while (xml.readNextStartElement())
    {
        if (xml.name() == KeySubsystemOutputConnection)
            success = loadSubsystemOutputConnectionItem(xml, components, outputs, outputconnections) && success;
        else
            xml.skipCurrentElement();
    }
    return success;
}

bool SystemXMLLoadModel::loadSubsystemOutputConnectionItem(QXmlStreamReader & xml,
                                                  SystemComponentMapInterface const * const components,
                                                  SubsystemOutputMapInterface const * const outputs,
                                                  SubsystemOutputConnectionMapInterface * const outputconnections)
{
    bool success = true;
    QList<QString> keys = KeyListSubsystemOutputConnection;
    SubsystemOutputConnectionMapInterface::ID id = 0;
    SystemComponentMapInterface::ID sourceComponentID = 0;
    SystemComponentOutputMapInterface::ID sourceOutputID = 0;
    SubsystemOutputConnectionItemInterface::ID targetOutputID = 0;

    while (xml.readNextStartElement())
    {
        keys.removeAll(xml.name().toString());
        if (xml.name() == KeySubsystemOutputConnectionID)
            id = xml.readElementText().toUInt();
        else if (xml.name() == KeySubsystemOutputConnectionSource)
            success = loadSystemConnectionItemSource(
                        xml, sourceComponentID, sourceOutputID) && success;
        else if (xml.name() == KeySubsystemOutputConnectionTarget)
            targetOutputID = xml.readElementText().toUInt();
        else
            xml.skipCurrentElement();
    }
    if (success && keys.isEmpty())
    {
       SystemComponentMapInterface::Item const * const sourceComponent =
                components->getItem(sourceComponentID);
       SubsystemOutputMapInterface::Item const * const target =
                outputs->getItem(targetOutputID);
        if (sourceComponent && target)
        {
            SystemConnectionMapInterface::Source const * const source =
                    sourceComponent->getOutputs()->getItem(sourceOutputID);
            if (source && target)
            {
                outputconnections->add(id, source, target);
            }
        }
        return true;
    }
    return false;
}

