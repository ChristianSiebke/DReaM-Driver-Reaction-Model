//-----------------------------------------------------------------------------
//! @file  SubsystemMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemMapInterface
//!        class. It contains the subsystem map as well as a reference to the component
//!        map (ComponentMapInterface). The latter is necessary to create the corresponding
//!        component item whenever a subsystem item is created.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMMAPMODEL_H
#define SUBSYSTEMMAPMODEL_H

#include "openPASS-System/SubsystemMapInterface.h"
#include "openPASS-System/SystemComponentManagerInterface.h"

/**
* @brief This model class implements the functionality of the SubsystemMapInterface class.
* It contains a reference to the subsystem map as well as to the component
* map (ComponentMapInterface). The latter is necessary to create the corresponding
* component item which represents the subsystem as a component block in systems or other subsystems.
**/
class SubsystemMapModel : public SubsystemMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Initializes the pointer to the component map such that corresponding component items can
    //! be created whenever subsystems are created. Sets the QObject as a parent if passed
    //! to the constructor as a second argument.
    //!
    //! @param[in]      componentMap    The reference to the component map
    //! @param[in]      parent          The reference to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemMapModel(ComponentMapInterface * const componentMap,
                               QObject * const parent = nullptr);
    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemMapModel() = default;

public:
    virtual SubsystemMapInterface::Iterator begin() override;
    virtual SubsystemMapInterface::ConstIterator begin() const override;

public:
    virtual SubsystemMapInterface::Iterator end() override;
    virtual SubsystemMapInterface::ConstIterator end() const override;



public:
    virtual SubsystemMapInterface::ID getID(SubsystemMapInterface::Item * const item) const override;
    virtual SubsystemMapInterface::Item * getItem(SubsystemMapInterface::ID const & id) const override;
    virtual SubsystemMapInterface::ID generateID() override;
    virtual SubsystemItemInterface::Title generateTitle() override;

public:
    virtual bool add(SubsystemMapInterface::ID const & id,
                     SubsystemMapInterface::Item::Component * const component) override;
    virtual bool add(SubsystemMapInterface::ID const & id,
                     SubsystemMapInterface::Item::Title const & name) override;

public:
    virtual QList <SubsystemMapInterface::Item *> values() override;
    virtual int count() const override;

public:
    virtual bool contains(SubsystemItemInterface::Title title) override;
    virtual bool contains(SubsystemItemInterface::ID const & id) override;

public:
    virtual SubsystemMapInterface::ID lastID() override;


public:
    virtual bool remove(SubsystemMapInterface::ID const & id) override;
    virtual bool clear() override;

protected:
    SubsystemMapInterface::Map subsystems;              //!< The map of subsystems
    ComponentMapInterface * const componentMap;         //!< The reference to the component map (initilialized to argument passed to the constructor)
};

#endif // SUBSYSTEMMAPMODEL_H
