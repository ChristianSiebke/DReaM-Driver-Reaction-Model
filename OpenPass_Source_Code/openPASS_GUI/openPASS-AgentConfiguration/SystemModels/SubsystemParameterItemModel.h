//-----------------------------------------------------------------------------
//! @file  SubsystemParameterItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemParameterItemInterface
//!        class.
//!
//!        It contains the attributes type (data type), title, unit, value and a pointer
//!        to the component parameter item which represents this parameter in a component. This allows
//!        for a synchronization of all changes made here with the corresponding attributes of the component
//!        parameter item. In addition to the counterpart classes for subsystem inputs and outputs
//!        (see SubsystemInputItemModel and SubsystemOutputItemModel), a pointer to
//!        the system component parameter item is included. This is because a subsystem parameter is nothing
//!        but a reference to a system component parameter inside the subsystem, which is made individually controllable from outside
//!        in the corresponding component block.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMPARAMETERITEMMODEL_H
#define SUBSYSTEMPARAMETERITEMMODEL_H

#include "openPASS-System/SubsystemParameterItemInterface.h"

//! @brief This model class implements the functionality of the SubsystemParameterItemInterface
//!        class.
//!
//!        It contains the attributes type (data type), title, unit, value and a pointer
//!        to the component parameter item which represents this parameter in a component. This allows
//!        for a synchronization of all changes made here with the corresponding attributes of the component
//!        parameter item. In addition to the counterpart classes for subsystem inputs and outputs
//!        (see SubsystemInputItemModel and SubsystemOutputItemModel), a pointer to
//!        the system component parameter item is included. This is because a subsystem parameter is nothing
//!        but a reference to a system component parameter inside the subsystem, which is made individually controllable from outside
//!        in the corresponding component blocks.
//-----------------------------------------------------------------------------
class SubsystemParameterItemModel : public SubsystemParameterItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! In the constructor, all attributes of this subsystem parameter are initialized to the values dictated by the
    //! system component parameter given as a second argument.
    //! If passed as second argument, the QObject is set as a parent
    //! (usually the parent will be an instance of SubsystemParameterMapModel).
    //!
    //! @param[in]      componentParameterItem          The pointer to the component parameter item
    //!                                                 representing this subsystem parameter in a component
    //! @param[in]      systemComponentParameterItem    The pointer to the system component parameter item to be addressed by
    //!                                                 this subsystem parameter
    //! @param[in]      parent                          The reference to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemParameterItemModel(ComponentParameterItemInterface * const componentParameterItem,
                                         SystemComponentParameterItemInterface * const systemComponentParameterItem,
                                         QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default desctructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemParameterItemModel() = default;

public:
    virtual bool setID(SubsystemParameterItemInterface::ID const & id) override;
    virtual SubsystemParameterItemInterface::ID getID() const override;

public:
    virtual SubsystemParameterItemInterface::Type getType() const override;
    virtual SubsystemParameterItemInterface::Unit getUnit() const override;

public:
    virtual bool setTitle(SubsystemParameterItemInterface::Title const & title) override;
    virtual SubsystemParameterItemInterface::Title getTitle() const override;


public:
    virtual bool setValue(SubsystemParameterItemInterface::Value const & value) override;
    virtual SubsystemParameterItemInterface::Value getValue() const override;

public:
    virtual SubsystemParameterItemInterface::ComponentParameterItem * getComponentParameter() const override;
    virtual SubsystemParameterItemInterface::SystemComponentParameterItem * getSystemComponentParameter() const override;

protected:
    SubsystemParameterItemInterface::ComponentParameterItem * const componentParameterItem;                 //!< The pointer to the component parameter representing this parameter in a component
    SubsystemParameterItemInterface::SystemComponentParameterItem * const systemComponentParameterItem;     //!< The pointer to the system component parameter to be addressed by this subsystem parameter

protected:
    SubsystemParameterItemInterface::Type type;         //!< the type of this subsystem parameter
    SubsystemParameterItemInterface::Title title;       //!< the title of this subsystem parameter
    SubsystemParameterItemInterface::Unit unit;         //!< the unit of this subsystem parameter
    SubsystemParameterItemInterface::Value value;       //!< the value of this subsystem parameter

};

#endif // SUBSYSTEMPARAMETERITEMMODEL_H
