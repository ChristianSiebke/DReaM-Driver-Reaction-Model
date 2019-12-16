//-----------------------------------------------------------------------------
//! @file  SubsystemOutputItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemOutputItemInterface
//!        class.
//!
//!        It contains the attributes type (data type), title and unit, their default
//!        values and a pointer to the component output item which represents this output in a component. This allows
//!        for a synchronization of all changes made here with the corresponding attributes of the component
//!        output item.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMOUTPUTITEMMODEL_H
#define SUBSYSTEMOUTPUTITEMMODEL_H

#include "openPASS-System/SubsystemOutputItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemOutputItemInterface
//!        class.
//!
//!        It contains the attributes type (data type), title and unit, their default
//!        values and a pointer to the component output item which represents this output in a component. This allows
//!        for a synchronization of all changes made here with the corresponding attributes of the component
//!        output item.
//-----------------------------------------------------------------------------
class SubsystemOutputItemModel : public SubsystemOutputItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! In the constructor, all attributes of this subsystem ouput and its corresponding component
    //! output are initialized to the default values. If passed as a second argument, the QObject is set as a parent
    //! (usually the parent will be an instance of SubsystemOutputMapModel).
    //!
    //! @param[in]      componentOutputItem     The reference to the component output item
    //!                                         representing this subsystem output
    //! @param[in]      parent                  The reference to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemOutputItemModel(ComponentOutputItemInterface * const componentOutputItem,
                                      QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default desctructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemOutputItemModel() = default;

public:
    virtual bool clear() override;

public:
    virtual bool setID(SubsystemOutputItemInterface::ID const & id) override;
    virtual SubsystemOutputItemInterface::ID getID() const override;

public:
    virtual bool setType(SubsystemOutputItemInterface::Type const & type) override;
    virtual SubsystemOutputItemInterface::Type getType() const override;

public:
    virtual bool setTitle(SubsystemOutputItemInterface::Title const & title) override;
    virtual SubsystemOutputItemInterface::Title getTitle() const override;

public:
    virtual bool setUnit(SubsystemOutputItemInterface::Unit const & unit) override;
    virtual SubsystemOutputItemInterface::Unit getUnit() const override;

public:
    virtual SubsystemOutputItemInterface::ComponentOutputItem * getComponentOutput() const override;

protected:
    static SubsystemOutputItemInterface::Type const DefaultType;                //!< The default data type (set to "double")
    static SubsystemOutputItemInterface::Title const DefaultTitle;              //!< The default title (set to "Output")
    static SubsystemOutputItemInterface::Unit const DefaultUnit;                //!< The default unit (set to "")

protected:
    SubsystemOutputItemInterface::ComponentOutputItem * const componentOutputItem;  //!< The pointer to the component output item representing this subsystem output


protected:
    SubsystemOutputItemInterface::Type type;            //!< The data type of this output
    SubsystemOutputItemInterface::Title title;          //!< The title of this output
    SubsystemOutputItemInterface::Unit unit;            //!< The unit of this output
};

#endif // SUBSYSTEMOUTPUTITEMMODEL_H
