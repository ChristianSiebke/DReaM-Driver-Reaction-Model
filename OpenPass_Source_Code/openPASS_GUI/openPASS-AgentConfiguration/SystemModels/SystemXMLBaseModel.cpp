#include "SystemXMLBaseModel.h"

QString const SystemXMLBaseModel::KeySystems = QStringLiteral("systems");
QString const SystemXMLBaseModel::KeySystem = QStringLiteral("system");
QString const SystemXMLBaseModel::KeySystemID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySystemTitle = QStringLiteral("title");
QString const SystemXMLBaseModel::KeySystemPriority = QStringLiteral("priority");
QString const SystemXMLBaseModel::KeySystemComponents = QStringLiteral("components");
QString const SystemXMLBaseModel::KeySystemComponent = QStringLiteral("component");
QString const SystemXMLBaseModel::KeySystemComponentID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySystemComponentLibrary = QStringLiteral("library");
QString const SystemXMLBaseModel::KeySystemComponentTitle = QStringLiteral("title");
QString const SystemXMLBaseModel::KeySystemComponentPriority = QStringLiteral("priority");
QString const SystemXMLBaseModel::KeySystemComponentSchedule = QStringLiteral("schedule");
QString const SystemXMLBaseModel::KeySystemComponentSchedulePriority = QStringLiteral("priority");
QString const SystemXMLBaseModel::KeySystemComponentScheduleOffset = QStringLiteral("offset");
QString const SystemXMLBaseModel::KeySystemComponentScheduleCycle = QStringLiteral("cycle");
QString const SystemXMLBaseModel::KeySystemComponentScheduleResponse = QStringLiteral("response");
QString const SystemXMLBaseModel::KeySystemComponentParameters = QStringLiteral("parameters");
QString const SystemXMLBaseModel::KeySystemComponentParameter = QStringLiteral("parameter");
QString const SystemXMLBaseModel::KeySystemComponentParameterID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySystemComponentParameterType = QStringLiteral("type");
QString const SystemXMLBaseModel::KeySystemComponentParameterUnit = QStringLiteral("unit");
QString const SystemXMLBaseModel::KeySystemComponentParameterValue = QStringLiteral("value");
QString const SystemXMLBaseModel::KeySystemComponentPosition = QStringLiteral("position");
QString const SystemXMLBaseModel::KeySystemComponentPositionX = QStringLiteral("x");
QString const SystemXMLBaseModel::KeySystemComponentPositionY = QStringLiteral("y");
QString const SystemXMLBaseModel::KeySystemConnections = QStringLiteral("connections");
QString const SystemXMLBaseModel::KeySystemConnection = QStringLiteral("connection");
QString const SystemXMLBaseModel::KeySystemConnectionID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySystemConnectionSource = QStringLiteral("source");
QString const SystemXMLBaseModel::KeySystemConnectionSourceComponent = QStringLiteral("component");
QString const SystemXMLBaseModel::KeySystemConnectionSourceOutput = QStringLiteral("output");
QString const SystemXMLBaseModel::KeySystemConnectionTarget = QStringLiteral("target");
QString const SystemXMLBaseModel::KeySystemConnectionTargetComponent = QStringLiteral("component");
QString const SystemXMLBaseModel::KeySystemConnectionTargetInput = QStringLiteral("input");

QString const SystemXMLBaseModel::KeySubsystemInputConnections = QStringLiteral("inputconnections");
QString const SystemXMLBaseModel::KeySubsystemInputConnection = QStringLiteral("inputconnection");
QString const SystemXMLBaseModel::KeySubsystemInputConnectionID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySubsystemInputConnectionSource = QStringLiteral("source");
QString const SystemXMLBaseModel::KeySubsystemInputConnectionTarget = QStringLiteral("target");
QString const SystemXMLBaseModel::KeySubsystemInputConnectionTargetComponent = QStringLiteral("component");
QString const SystemXMLBaseModel::KeySubsystemInputConnectionTargetInput = QStringLiteral("input");


QString const SystemXMLBaseModel::KeySubsystemOutputConnections = QStringLiteral("outputconnections");
QString const SystemXMLBaseModel::KeySubsystemOutputConnection = QStringLiteral("outputconnection");
QString const SystemXMLBaseModel::KeySubsystemOutputConnectionID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySubsystemOutputConnectionTarget = QStringLiteral("target");
QString const SystemXMLBaseModel::KeySubsystemOutputConnectionSource = QStringLiteral("source");
QString const SystemXMLBaseModel::KeySubsystemOutputConnectionSourceComponent = QStringLiteral("component");
QString const SystemXMLBaseModel::KeySubsystemOutputConnectionSourceOutput = QStringLiteral("output");


QString const SystemXMLBaseModel::KeySubsystems = QStringLiteral("subsystems");
QString const SystemXMLBaseModel::KeySubsystem = QStringLiteral("subsystem");
QString const SystemXMLBaseModel::KeySubsystemID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySubsystemTitle = QStringLiteral("title");
//QString const SystemXMLBaseModel::KeySubsystemSchedule = QStringLiteral("schedule");

QString const SystemXMLBaseModel::KeySubsystemInputs = QStringLiteral("inputs");
QString const SystemXMLBaseModel::KeySubsystemInput = QStringLiteral("input");
QString const SystemXMLBaseModel::KeySubsystemInputID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySubsystemInputTitle = QStringLiteral("title");
QString const SystemXMLBaseModel::KeySubsystemInputType = QStringLiteral("type");
QString const SystemXMLBaseModel::KeySubsystemInputUnit = QStringLiteral("unit");

QString const SystemXMLBaseModel::KeySubsystemOutputs = QStringLiteral("outputs");
QString const SystemXMLBaseModel::KeySubsystemOutput = QStringLiteral("output");
QString const SystemXMLBaseModel::KeySubsystemOutputID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySubsystemOutputTitle = QStringLiteral("title");
QString const SystemXMLBaseModel::KeySubsystemOutputType = QStringLiteral("type");
QString const SystemXMLBaseModel::KeySubsystemOutputUnit = QStringLiteral("unit");

QString const SystemXMLBaseModel::KeySubsystemParameters = QStringLiteral("parameters");
QString const SystemXMLBaseModel::KeySubsystemParameter = QStringLiteral("parameter");
QString const SystemXMLBaseModel::KeySubsystemParameterID = QStringLiteral("id");
QString const SystemXMLBaseModel::KeySubsystemParameterTitle = QStringLiteral("title");
QString const SystemXMLBaseModel::KeySubsystemParameterType = QStringLiteral("type");
QString const SystemXMLBaseModel::KeySubsystemParameterUnit = QStringLiteral("unit");
QString const SystemXMLBaseModel::KeySubsystemParameterValue = QStringLiteral("value");
QString const SystemXMLBaseModel::KeySubsystemParameterSource = QStringLiteral("source");
QString const SystemXMLBaseModel::KeySubsystemParameterSourceComponent = QStringLiteral("component");
QString const SystemXMLBaseModel::KeySubsystemParameterSourceParameter = QStringLiteral("parameter");

QList<QString> const SystemXMLBaseModel::KeyListSystem = {
    SystemXMLBaseModel::KeySystemID,
    SystemXMLBaseModel::KeySystemTitle,
    SystemXMLBaseModel::KeySystemPriority,
    SystemXMLBaseModel::KeySystemComponents,
    SystemXMLBaseModel::KeySystemConnections,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemComponent = {
    SystemXMLBaseModel::KeySystemComponentID,
    SystemXMLBaseModel::KeySystemComponentLibrary,
    SystemXMLBaseModel::KeySystemComponentTitle,
    SystemXMLBaseModel::KeySystemComponentSchedule,
    SystemXMLBaseModel::KeySystemComponentParameters,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemComponentSchedule = {
    SystemXMLBaseModel::KeySystemComponentScheduleOffset,
    SystemXMLBaseModel::KeySystemComponentScheduleCycle,
    SystemXMLBaseModel::KeySystemComponentScheduleResponse,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemComponentParameter = {
    SystemXMLBaseModel::KeySystemComponentParameterID,
    SystemXMLBaseModel::KeySystemComponentParameterType,
    SystemXMLBaseModel::KeySystemComponentParameterUnit,
    SystemXMLBaseModel::KeySystemComponentParameterValue,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemComponentPosition = {
    SystemXMLBaseModel::KeySystemComponentPositionX,
    SystemXMLBaseModel::KeySystemComponentPositionY,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemConnection = {
    SystemXMLBaseModel::KeySystemConnectionID,
    SystemXMLBaseModel::KeySystemConnectionSource,
    SystemXMLBaseModel::KeySystemConnectionTarget,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemConnectionSource = {
    SystemXMLBaseModel::KeySystemConnectionSourceComponent,
    SystemXMLBaseModel::KeySystemConnectionSourceOutput,
};
QList<QString> const SystemXMLBaseModel::KeyListSystemConnectionTarget = {
    SystemXMLBaseModel::KeySystemConnectionTargetComponent,
    SystemXMLBaseModel::KeySystemConnectionTargetInput,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemInputConnection = {
    SystemXMLBaseModel::KeySubsystemInputConnectionID,
    SystemXMLBaseModel::KeySubsystemInputConnectionSource,
    SystemXMLBaseModel::KeySubsystemInputConnectionTarget,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemOutputConnection = {
    SystemXMLBaseModel::KeySubsystemOutputConnectionID,
    SystemXMLBaseModel::KeySubsystemOutputConnectionSource,
    SystemXMLBaseModel::KeySubsystemOutputConnectionTarget,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemInputConnectionTarget = {
    SystemXMLBaseModel::KeySubsystemInputConnectionTargetComponent,
    SystemXMLBaseModel::KeySubsystemInputConnectionTargetInput,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemOutputConnectionSource = {
    SystemXMLBaseModel::KeySubsystemOutputConnectionSourceComponent,
    SystemXMLBaseModel::KeySubsystemOutputConnectionSourceOutput,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemInput = {
    SystemXMLBaseModel::KeySubsystemInputID,
    SystemXMLBaseModel::KeySubsystemInputTitle,
    SystemXMLBaseModel::KeySubsystemInputType,
    SystemXMLBaseModel::KeySubsystemInputUnit,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemOutput = {
    SystemXMLBaseModel::KeySubsystemOutputID,
    SystemXMLBaseModel::KeySubsystemOutputTitle,
    SystemXMLBaseModel::KeySubsystemOutputType,
    SystemXMLBaseModel::KeySubsystemOutputUnit,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemParameter = {
    SystemXMLBaseModel::KeySubsystemParameterID,
    SystemXMLBaseModel::KeySubsystemParameterTitle,
    SystemXMLBaseModel::KeySubsystemParameterType,
    SystemXMLBaseModel::KeySubsystemParameterUnit,
    SystemXMLBaseModel::KeySubsystemParameterValue,
    SystemXMLBaseModel::KeySubsystemParameterSource,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystemParameterSource = {
    SystemXMLBaseModel::KeySubsystemParameterSourceComponent,
    SystemXMLBaseModel::KeySubsystemParameterSourceParameter,
};

QList<QString> const SystemXMLBaseModel::KeyListSubsystem = {
    SystemXMLBaseModel::KeySubsystemID,
    SystemXMLBaseModel::KeySubsystemTitle,
    SystemXMLBaseModel::KeySubsystemParameters,
    SystemXMLBaseModel::KeySubsystemInputs,
    SystemXMLBaseModel::KeySubsystemOutputs,
    SystemXMLBaseModel::KeySystemComponents,
    SystemXMLBaseModel::KeySystemConnections,
};
