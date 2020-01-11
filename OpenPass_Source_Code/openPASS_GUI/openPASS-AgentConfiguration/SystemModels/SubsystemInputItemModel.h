//-----------------------------------------------------------------------------
//! @file  SubsystemIntputItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemInputItemInterface
//!        class. It contains the attributes type (data type), title and unit, their default
//!        values and a pointer to the component input item which represents this input in a component. This allows
//!        for a synchronization of all changes made here with the corresponding attributes of the component
//!        input item.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMINPUTITEMMODEL_H
#define SUBSYSTEMINPUTITEMMODEL_H

#include "openPASS-System/SubsystemInputItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemInputItemInterface
//!        class. It contains the attributes type (data type), title and unit, their default
//!        values and a pointer to the component input item which represents this input in a component. This allows
//!        for a synchronization of all changes made here with the corresponding attributes of the component
//!        input item.
//-----------------------------------------------------------------------------
class SubsystemInputItemModel : public SubsystemInputItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! In the constructor, all attributes of this subsystem ouput and its corresponding component
    //! input are initialized to the default values. If passed as second argument, the QObject is set as a parent
    //! (usually the parent will be an instance of SubsystemInputMapModel).
    //!
    //! @param[in]      componentInputItem     The reference to the component input item
    //!                                         representing this subsystem input
    //! @param[in]      parent                  The reference to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemInputItemModel(ComponentInputItemInterface * const componentInputItem,
                                     QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default desctructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemInputItemModel() = default;

public:
    virtual bool clear() override;

public:
    virtual bool setID(SubsystemInputItemInterface::ID const & id) override;
    virtual SubsystemInputItemInterface::ID getID() const override;

public:
    virtual bool setType(SubsystemInputItemInterface::Type const & type) override;
    virtual SubsystemInputItemInterface::Type getType() const override;

public:
    virtual bool setTitle(SubsystemInputItemInterface::Title const & title) override;
    virtual SubsystemInputItemInterface::Title getTitle() const override;

public:
    virtual bool setUnit(SubsystemInputItemInterface::Unit const & unit) override;
    virtual SubsystemInputItemInterface::Unit getUnit() const override;

public:
    virtual SubsystemInputItemInterface::ComponentInputItem * getComponentInput() const override;

protected:
    static SubsystemInputItemInterface::Type const DefaultType;             //!< The default data type (set to "double")
    static SubsystemInputItemInterface::Title const DefaultTitle;           //!< The default title (set to "Input")
    static SubsystemInputItemInterface::Unit const DefaultUnit;             //!< The default unit (set to "")

protected:
    SubsystemInputItemInterface::ComponentInputItem * const componentInputItem;     //!< The pointer to the component input item representing this subsystem input

protected:
    SubsystemInputItemInterface::Type type;                 //!< The data type of this input
    SubsystemInputItemInterface::Title title;               //!< The title of this input
    SubsystemInputItemInterface::Unit unit;                 //!< The unit of this input
};

#endif // SUBSYSTEMINPUTITEMMODEL_H
