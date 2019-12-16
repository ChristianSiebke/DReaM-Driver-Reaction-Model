//-----------------------------------------------------------------------------
//! @file  SystemXMLSaveModel.h
//! @ingroup systemPlugin
//! @brief The SystemXMLSaveModel class contains methods for saving a systemConfig
//!        XML-file.
//!
//!        The save-procudure is subdivided into several subroutines whose names make
//!        clear which part of the XML file is addressed.
//!
//-----------------------------------------------------------------------------

#ifndef SYSTEMXMLSAVEMODEL_H
#define SYSTEMXMLSAVEMODEL_H

#include "SystemXMLBaseModel.h"

class QIODevice;
class QXmlStreamWriter;
class SystemComponentMapInterface;
class SystemComponentParameterMapInterface;
class SystemComponentScheduleInterface;
class SystemConnectionMapInterface;
class SystemItemInterface;
class SystemMapInterface;


class SubsystemMapInterface;
class SubsystemItemInterface;
class SubsystemInputMapInterface;
class SubsystemOutputMapInterface;
class SubsystemParameterMapInterface;
class SubsystemInputConnectionMapInterface;
class SubsystemOutputConnectionMapInterface;

//-----------------------------------------------------------------------------
//! @brief The SystemXMLSaveModel class contains methods for saving a systemConfig
//!        XML-file.
//!
//!        The save-procudure is subdivided into several subroutines whose names make
//!        clear which part of the XML file is addressed.
//!
//-----------------------------------------------------------------------------
class SystemXMLSaveModel : public SystemXMLBaseModel
{
public:
    explicit SystemXMLSaveModel() = delete;
    virtual ~SystemXMLSaveModel() = default;

public:
    static bool save(QString const & filepath,
                     SystemMapInterface const * const systems,
                     SubsystemMapInterface const * const subsystems);
    static bool save(QIODevice * const device,
                     SystemMapInterface const * const systems,
                     SubsystemMapInterface const * const subsystems);

protected:

    static void saveSystem(QXmlStreamWriter & xml,
                           SystemItemInterface const * const system);
    static void saveSystemComponent(QXmlStreamWriter & xml,
                                    SystemComponentMapInterface const * const components);
    static void saveSystemComponentSchedule(QXmlStreamWriter & xml,
                                            SystemComponentScheduleInterface const * const schedule);
    static void saveSystemComponentParameters(QXmlStreamWriter & xml,
                                              SystemComponentParameterMapInterface const * const parameters);
    static void saveSystemConnection(QXmlStreamWriter & xml,
                                     SystemConnectionMapInterface const * const connections);

    static void saveSubsystemInputConnection(QXmlStreamWriter & xml,
                                     SubsystemInputConnectionMapInterface const * const inputconnections);

    static void saveSubsystemOutputConnection(QXmlStreamWriter & xml,
                                     SubsystemOutputConnectionMapInterface const * const outputconnections);




    static void saveSubsystem(QXmlStreamWriter & xml,
                              SubsystemItemInterface const * const subsystem);

    static void saveSubsystemParameters(QXmlStreamWriter & xml,
                                        SubsystemParameterMapInterface const * const parameters);

    static void saveSubsystemInputs(QXmlStreamWriter & xml,
                                        SubsystemInputMapInterface const * const inputs);

    static void saveSubsystemOutputs(QXmlStreamWriter & xml,
                                        SubsystemOutputMapInterface const * const outputs);
};

#endif // SYSTEMXMLSAVEMODEL_H
