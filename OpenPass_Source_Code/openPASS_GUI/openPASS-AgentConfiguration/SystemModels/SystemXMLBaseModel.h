#ifndef SYSTEMXMLBASEMODEL_H
#define SYSTEMXMLBASEMODEL_H

#include <QList>
#include <QObject>
#include <QString>

class SystemXMLBaseModel
{
public:
    explicit SystemXMLBaseModel() = delete;
    virtual ~SystemXMLBaseModel() = default;

protected:
    static QString const KeySystems;
    static QString const KeySystem;
    static QString const KeySystemID;
    static QString const KeySystemTitle;
    static QString const KeySystemPriority;
    static QString const KeySystemComponents;
    static QString const KeySystemComponent;
    static QString const KeySystemComponentID;
    static QString const KeySystemComponentLibrary;
    static QString const KeySystemComponentTitle;
    static QString const KeySystemComponentPriority;
    static QString const KeySystemComponentSchedule;
    static QString const KeySystemComponentSchedulePriority;
    static QString const KeySystemComponentScheduleOffset;
    static QString const KeySystemComponentScheduleCycle;
    static QString const KeySystemComponentScheduleResponse;
    static QString const KeySystemComponentParameters;
    static QString const KeySystemComponentParameter;
    static QString const KeySystemComponentParameterID;
    static QString const KeySystemComponentParameterType;
    static QString const KeySystemComponentParameterUnit;
    static QString const KeySystemComponentParameterValue;
    static QString const KeySystemComponentPosition;
    static QString const KeySystemComponentPositionX;
    static QString const KeySystemComponentPositionY;
    static QString const KeySystemConnections;
    static QString const KeySystemConnection;
    static QString const KeySystemConnectionID;
    static QString const KeySystemConnectionSource;
    static QString const KeySystemConnectionSourceComponent;
    static QString const KeySystemConnectionSourceOutput;
    static QString const KeySystemConnectionTarget;
    static QString const KeySystemConnectionTargetComponent;
    static QString const KeySystemConnectionTargetInput;

    static QString const KeySubsystemInputConnections;
    static QString const KeySubsystemInputConnection;
    static QString const KeySubsystemInputConnectionID;
    static QString const KeySubsystemInputConnectionSource;
    static QString const KeySubsystemInputConnectionTarget;
    static QString const KeySubsystemInputConnectionTargetComponent;
    static QString const KeySubsystemInputConnectionTargetInput;

    static QString const KeySubsystemOutputConnections;
    static QString const KeySubsystemOutputConnection;
    static QString const KeySubsystemOutputConnectionID;
    static QString const KeySubsystemOutputConnectionTarget;
    static QString const KeySubsystemOutputConnectionSource;
    static QString const KeySubsystemOutputConnectionSourceComponent;
    static QString const KeySubsystemOutputConnectionSourceOutput;

    static QString const KeySubsystems;
    static QString const KeySubsystem;
    static QString const KeySubsystemID;
    static QString const KeySubsystemTitle;
//    static QString const KeySubsystemSchedule;
    static QString const KeySubsystemParameters;
    static QString const KeySubsystemInputs;
    static QString const KeySubsystemOutputs;
    static QString const KeySubsystemComponents;
    static QString const KeySubsystemConnections;


    static QString const KeySubsystemInput;
    static QString const KeySubsystemInputID;
    static QString const KeySubsystemInputTitle;
    static QString const KeySubsystemInputType;
    static QString const KeySubsystemInputUnit;


    static QString const KeySubsystemOutput;
    static QString const KeySubsystemOutputID;
    static QString const KeySubsystemOutputTitle;
    static QString const KeySubsystemOutputType;
    static QString const KeySubsystemOutputUnit;


    static QString const KeySubsystemParameter;
    static QString const KeySubsystemParameterID;
    static QString const KeySubsystemParameterTitle;
    static QString const KeySubsystemParameterSource;
    static QString const KeySubsystemParameterSourceComponent;
    static QString const KeySubsystemParameterSourceParameter;
    static QString const KeySubsystemParameterType;
    static QString const KeySubsystemParameterUnit;
    static QString const KeySubsystemParameterValue;


protected:
    static QList<QString> const KeyListSystem;
    static QList<QString> const KeyListSystemComponent;
    static QList<QString> const KeyListSystemComponentSchedule;
    static QList<QString> const KeyListSystemComponentParameter;
    static QList<QString> const KeyListSystemComponentPosition;
    static QList<QString> const KeyListSystemConnection;
    static QList<QString> const KeyListSystemConnectionSource;
    static QList<QString> const KeyListSystemConnectionTarget;

    static QList<QString> const KeyListSubsystemInputConnection;
    static QList<QString> const KeyListSubsystemInputConnectionTarget;

    static QList<QString> const KeyListSubsystemOutputConnection;
    static QList<QString> const KeyListSubsystemOutputConnectionSource;

    static QList<QString> const KeyListSubsystem;
    static QList<QString> const KeyListSubsystemInput;
    static QList<QString> const KeyListSubsystemOutput;
    static QList<QString> const KeyListSubsystemParameter;

   static QList<QString> const KeyListSubsystemParameterSource;

};

#endif // SYSTEMXMLBASEMODEL_H
