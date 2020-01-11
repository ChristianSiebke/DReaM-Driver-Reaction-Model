//-----------------------------------------------------------------------------
//! @file  SystemItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SystemItemInterface
//!        class. It contains the references to the maps of connections and system components.
//-----------------------------------------------------------------------------

#ifndef SYSTEMITEMMODEL_H
#define SYSTEMITEMMODEL_H

#include "openPASS-System/SystemItemInterface.h"

class SystemComponentManagerInterface;


/**
* @brief This model class implements the functionality of the SystemItemInterface class.
*  It contains the references to the maps of connections and system components.
**/
class SystemItemModel : public SystemItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Initializes all attributes of this system, i.e. it creates all maps.
    //! It also sets the QObject as a parent if passed as a second argument.
    //!
    //! @param[in]      component       The reference to the component item
    //!                                 representing this subsystem
    //! @param[in]      parent          The reference to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SystemItemModel(QObject * const parent = nullptr);
    virtual ~SystemItemModel() = default;

public:
    virtual SystemItemInterface::ID getID() const override;

public:
    virtual bool setTitle(SystemItemInterface::Title const & title) override;
    virtual SystemItemInterface::Title getTitle() const override;

public:
    virtual bool setPriority(SystemItemInterface::Priority const & priority) override;
    virtual SystemItemInterface::Priority getPriority() const override;

public:
    virtual SystemItemInterface::ComponentMap * getComponents() const override;
    virtual SystemItemInterface::ConnectionMap * getConnections() const override;

public:
    virtual QList<SystemConnectionMapInterface::Source const *> getCompatibleSources(
            SystemConnectionMapInterface::Target const * const target) override;
    virtual QList<SystemConnectionMapInterface::Target const *> getCompatibleTargets(
            SystemConnectionMapInterface::Source const * const source) override;


protected:
    SystemItemInterface::Title title;                           //!< The system's title
    SystemItemInterface::Priority priority;                     //!< The system's priority
    SystemItemInterface::ConnectionMap * const connections;     //!< The pointer to the connection map (connections between components)
    SystemItemInterface::ComponentMap * const components;       //!< The pointer to the map of system components in this system
};

#endif // SYSTEMITEMMODEL_H
