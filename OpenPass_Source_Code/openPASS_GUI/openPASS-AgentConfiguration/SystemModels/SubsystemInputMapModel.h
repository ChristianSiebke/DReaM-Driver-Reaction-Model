//-----------------------------------------------------------------------------
//! @file  SubsystemInputMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemInputMapInterface
//!        class. It contains a reference to the subsystem input map as well as to the component
//!        input map (ComponentInputMapInterface). The latter is needed to create component inputs
//!        whenever subsystem inputs are created.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMINPUTMAPMODEL_H
#define SUBSYSTEMINPUTMAPMODEL_H

#include "openPASS-System/SubsystemInputMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemInputMapInterface
//!        class. It contains a reference to the subsystem input map as well as to the component
//!        input map (ComponentInputMapInterface). The latter is needed to create component inputs
//!        whenever subsystem inputs are created.
//-----------------------------------------------------------------------------
class SubsystemInputMapModel : public SubsystemInputMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Initializes the pointer to the component input map such that corresponding component inputs can
    //! be created whenever subsystem inputs are created. Sets the QObject as a parent if passed
    //! to the constructor as a second argument.
    //!
    //! @param[in]      componentInputs   The pointer to the component input map
    //! @param[in]      parent            The pointer to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemInputMapModel(ComponentInputMapInterface * const componentInputs,
                                    QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemInputMapModel() = default;

public:
    virtual bool add(SubsystemInputMapInterface::ID const & id) override;
    virtual bool add(SubsystemInputMapInterface::ID const & id,
                     SubsystemInputMapInterface::Item * const item) override;

public:
    virtual SubsystemInputMapInterface::Iterator begin() override;
    virtual SubsystemInputMapInterface::ConstIterator begin() const override;

public:
    virtual bool clear() override;

public:
    virtual int count() const override;

public:
    virtual bool contains(SubsystemInputMapInterface::ID const & id) const override;
    virtual bool contains(SubsystemInputMapInterface::Item * const item) const override;
    virtual bool contains(SubsystemInputMapInterface::Index const & index) const override;

public:
    virtual SubsystemInputMapInterface::Iterator end() override;
    virtual SubsystemInputMapInterface::ConstIterator end() const override;

public:
    virtual bool setID(SubsystemInputMapInterface::Item * const item,
                       SubsystemInputMapInterface::ID const & id) override;
    virtual SubsystemInputMapInterface::ID generateID() const override;
    virtual SubsystemInputMapInterface::ID getID(SubsystemInputMapInterface::Item * const item) const override;
    virtual SubsystemInputMapInterface::ID getID(SubsystemInputMapInterface::Index const & index) const override;

public:
    virtual SubsystemInputMapInterface::Index getIndex(SubsystemInputMapInterface::ID const & id) const override;
    virtual SubsystemInputMapInterface::Index getIndex(SubsystemInputMapInterface::Item * const item) const override;

public:
    virtual SubsystemInputMapInterface::Item * getItem(SubsystemInputMapInterface::ID const & id) const override;
    virtual SubsystemInputMapInterface::Item * getItem(SubsystemInputMapInterface::Index const & index) const override;

public:
    virtual bool remove(SubsystemInputMapInterface::ID const & id) override;
    virtual bool remove(SubsystemInputMapInterface::Item * const item) override;
    virtual bool remove(SubsystemInputMapInterface::Index const & index) override;

protected:
    SubsystemInputMapInterface::Map map;                                    //!< The map of subsystem input items
    SubsystemInputMapInterface::ComponentInputs * const componentInputs;    //!< The pointer to the component input map (initialized to the argument passed to the constructor)
};

#endif // SUBSYSTEMINPUTMAPMODEL_H
