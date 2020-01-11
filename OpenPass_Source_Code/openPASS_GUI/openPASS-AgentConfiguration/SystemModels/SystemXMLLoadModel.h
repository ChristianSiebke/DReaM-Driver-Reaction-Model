//-----------------------------------------------------------------------------
//! @file  SystemXMLLoadModel.h
//! @ingroup systemPlugin
//! @brief The SystemXMLLoadModel class contains methods for loading a systemConfig
//!        XML-file.
//!
//!        The load-routine is subdivided into several subroutines whose names make clear which
//!        part of the XML file is loaded. Before the full XML data is used to create systems
//!        and subsystems, a pre-scan needs to be performed to detect all subsystems. Based on
//!        this pre-scan, the component items (ComponentItemInterface) are generated such that
//!        a subsystem is known already before a system incorporates it as a system component.
//!
//-----------------------------------------------------------------------------

#ifndef SYSTEMXMLLOADMODEL_H
#define SYSTEMXMLLOADMODEL_H

#include "openPASS-System/SystemComponentInputMapInterface.h"
#include "openPASS-System/SystemComponentItemInterface.h"
#include "openPASS-System/SystemComponentManagerInterface.h"
#include "openPASS-System/SystemComponentMapInterface.h"
#include "openPASS-System/SystemComponentOutputMapInterface.h"
#include "openPASS-System/SystemComponentParameterMapInterface.h"
#include "openPASS-System/SystemComponentScheduleInterface.h"
#include "openPASS-System/SystemConnectionMapInterface.h"
#include "openPASS-System/SystemItemInterface.h"
#include "openPASS-System/SystemMapInterface.h"
#include "SystemXMLBaseModel.h"

class QIODevice;
class QXmlStreamReader;


//-----------------------------------------------------------------------------
//! @brief The SystemXMLLoadModel class contains methods for loading a systemConfig
//!        XML-file.
//!
//!        The load-routine is spread over several subroutines whose name makes clear which
//!        part of the XML file is loaded. Before the full XML data is used to create systems
//!        and subsystems, a pre-scan needs to be performed to detect all subsystems. Based on
//!        this pre-scan, the component items (ComponentItemInterface) are generated such that
//!        a subsystem is known already before a system incorporates it as a system component.
//!
//-----------------------------------------------------------------------------
class SystemXMLLoadModel : public SystemXMLBaseModel
{
public:
    explicit SystemXMLLoadModel() = delete;
    virtual ~SystemXMLLoadModel() = default;

public:
    static bool load(QString const & filepath,
                     SystemMapInterface * const systems,
                     SubsystemMapInterface *const subsystems,
                     SystemComponentManagerInterface const * const manager);


    static bool load(QIODevice * const device,
                     SystemMapInterface * const systems,
                     SubsystemMapInterface * const subsystems,
                     SystemComponentManagerInterface const * const manager);

protected:
    static bool loadSystemItem(QXmlStreamReader & xml,
                                   SystemMapInterface * const systems,
                                   SystemComponentManagerInterface const * const manager);

protected:
   static bool registerSubsystems(QXmlStreamReader & xml,
                            SystemComponentManagerInterface const * const manager);

   static bool registerSubsystemItem(QXmlStreamReader & xml,
                               SystemComponentManagerInterface const * const manager);

   static bool registerSubsystemParameters(QXmlStreamReader & xml,
                                           ComponentItemInterface * component);

   static bool registerSubsystemInputs(QXmlStreamReader & xml,
                                           ComponentItemInterface * component);

   static bool registerSubsystemOutputs(QXmlStreamReader & xml,
                                           ComponentItemInterface * component);


protected:
   static bool loadSubsystemItem(QXmlStreamReader & xml,
                                 SubsystemMapInterface * const subsystems,
                                 SystemComponentManagerInterface const * const manager);

    static bool loadSubsystemParameterMap(QXmlStreamReader & xml,
                                          ComponentParameterMapInterface * const componentParameters,
                                          SubsystemParameterMapInterface * const parameters,
                                          SystemComponentMapInterface * const components);

    static bool loadSubsystemInputMap(QXmlStreamReader & xml,
                                         SubsystemInputMapInterface * const inputs);
    static bool loadSubsystemOutputMap(QXmlStreamReader & xml,
                                         SubsystemOutputMapInterface * const parameters);

    static bool loadSubsystemParameterItem(QXmlStreamReader & xml,
                                           ComponentParameterMapInterface * const componentParameters,
                                           SubsystemParameterMapInterface * const parameters,
                                           SystemComponentMapInterface * const components);

    static bool loadSubsystemInputItem(QXmlStreamReader &xml,
                                            SubsystemInputMapInterface * const inputs);
    static bool loadSubsystemOutputItem(QXmlStreamReader &xml,
                                            SubsystemOutputMapInterface * const outputs);


protected:
    static bool loadSystemComponentMap(QXmlStreamReader & xml,
                                       SystemComponentMapInterface * const components,
                                       SystemComponentManagerInterface const * const manager);
    static bool loadSystemComponentItem(QXmlStreamReader & xml,
                                        SystemComponentMapInterface * const components,
                                        SystemComponentManagerInterface const * const manager);
    static bool loadSystemComponentSchedule(QXmlStreamReader & xml,
                                            SystemComponentScheduleInterface * const schedule);
    static bool loadSystemComponentParameterMap(QXmlStreamReader & xml,
                                                SystemComponentParameterMapInterface * const parameters);
    static bool loadSystemComponentParameterItem(QXmlStreamReader & xml,
                                                 SystemComponentParameterMapInterface * const parameters);
    static bool loadSystemComponentPosition(QXmlStreamReader & xml,
                                            SystemComponentItemInterface * const component);

protected:
    static bool loadSystemConnectionMap(QXmlStreamReader & xml,
                                        SystemComponentMapInterface const * const components,
                                        SystemConnectionMapInterface * const connections);
    static bool loadSystemConnectionItem(QXmlStreamReader & xml,
                                         SystemComponentMapInterface const * const components,
                                         SystemConnectionMapInterface * const connections);
    static bool loadSystemConnectionItemSource(QXmlStreamReader & xml,
                                               SystemComponentMapInterface::ID & sourceComponentID,
                                               SystemComponentOutputMapInterface::ID & sourceOutputID);
    static bool loadSystemConnectionItemTarget(QXmlStreamReader & xml,
                                               SystemComponentMapInterface::ID & targetComponentID,
                                               SystemComponentInputMapInterface::ID & targetInputID);

protected:
    static bool loadSubsystemInputConnectionMap(QXmlStreamReader & xml,
                                                 SystemComponentMapInterface const * const components,
                                                 SubsystemInputMapInterface const * const inputs,
                                                 SubsystemInputConnectionMapInterface * const inputconnections);
    static bool loadSubsystemInputConnectionItem(QXmlStreamReader & xml,
                                                 SystemComponentMapInterface const * const components,
                                                 SubsystemInputMapInterface const * const inputs,
                                                 SubsystemInputConnectionMapInterface * const inputconnections);
    static bool loadSubsystemOutputConnectionMap(QXmlStreamReader & xml,
                                                 SystemComponentMapInterface const * const components,
                                                 SubsystemOutputMapInterface const * const outputs,
                                                 SubsystemOutputConnectionMapInterface * const outputconnections);
    static bool loadSubsystemOutputConnectionItem(QXmlStreamReader & xml,
                                                 SystemComponentMapInterface const * const components,
                                                 SubsystemOutputMapInterface const * const outputs,
                                                 SubsystemOutputConnectionMapInterface * const outputconnections);
};

#endif // SYSTEMXMLLOADMODEL_H
