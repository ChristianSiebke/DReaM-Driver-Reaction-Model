//-----------------------------------------------------------------------------
//! @file  SystemErrorModel.h
//! @ingroup systemPlugin
//! @brief This class implements the functionality of SystemErrorInterface.
//!
//!        It contains a reference to the SystemInterface containing all
//!        XML elements of the systemConfig. Further, an error map is created
//!        in which errors are registered according to the error types.
//-----------------------------------------------------------------------------

#ifndef SYSTEMERRORMODEL_H
#define SYSTEMERRORMODEL_H

#include "openPASS-System/SystemErrorInterface.h"

#include <QMap>
#include <QStringList>

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of SystemErrorInterface.
//!
//!        It contains a reference to the SystemInterface containing all
//!        XML elements of the systemConfig. Further, an error map is created
//!        in which errors are registered according to the error types.
//-----------------------------------------------------------------------------
class SystemErrorModel : public SystemErrorInterface
{

    Q_OBJECT

public:
    explicit SystemErrorModel(SystemInterface * const system,
                              QObject * const parent = nullptr);
    virtual ~SystemErrorModel()=default;


public:
    virtual QString getErrorReport() override;
    virtual void clearErrors() override;

public:
    virtual void checkConnection(SystemConnectionItemInterface const * const connection) override;
    virtual void checkInputConnection(SubsystemInputConnectionItemInterface const * const connection) override;
    virtual void checkOutputConnection(SubsystemOutputConnectionItemInterface const * const connection) override;

public:
    virtual void checkComponentInputs(SystemComponentItemInterface const * const systemComponent) override;

public:
    virtual void checkSystemItem(SystemItemInterface const * const system) override;
    virtual void checkSubsystemItem(SubsystemItemInterface const * const subsystem) override;

public:
    virtual void checkSystem() override;


private:
    SystemInterface * const system;         //!< The pointer to the SystemInterface
    QMap<Type,QStringList> errors;          //!< The map in which all error codes are registered. Error codes consist of QStrings. Hence, the error types are mapped onto a string list of all error strings belonging to that type.

};

#endif // SYSTEMERRORMODEL_H
