//-----------------------------------------------------------------------------
//! @file  SubsystemOutputMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemOutputMapInterface
//!        class. It contains a reference to the subsystem output map as well as to the component
//!        output map (ComponentOutputMapInterface). The latter is needed to create component outputs
//!        whenever subsystem outputs are created.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMOUTPUTMAPMODEL_H
#define SUBSYSTEMOUTPUTMAPMODEL_H

#include "openPASS-System/SubsystemOutputMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemOutputMapInterface
//!        class. It contains a reference to the subsystem output map as well as to the component
//!        output map (ComponentOutputMapInterface). The latter is needed to create component outputs
//!        whenever subsystem outputs are created.
//-----------------------------------------------------------------------------
class SubsystemOutputMapModel : public SubsystemOutputMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Initializes the pointer to the component output map such that corresponding component outputs can
    //! be created whenever subsystem outputs are created. Sets the QObject as a parent if passed
    //! to the constructor as a second argument (usually the parent will be an instance of SubsystemItemModel).
    //!
    //! @param[in]      componentOutputs   The pointer to the component output map
    //! @param[in]      parent             The pointer to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemOutputMapModel(ComponentOutputMapInterface * const componentOutputs,
                                     QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemOutputMapModel() = default;

public:
    virtual bool add(SubsystemOutputMapInterface::ID const & id) override;
    virtual bool add(SubsystemOutputMapInterface::ID const & id,
                     SubsystemOutputMapInterface::Item * const item) override;

public:
    virtual SubsystemOutputMapInterface::Iterator begin() override;
    virtual SubsystemOutputMapInterface::ConstIterator begin() const override;

public:
    virtual bool clear() override;

public:
    virtual int count() const override;

public:
    virtual bool contains(SubsystemOutputMapInterface::ID const & id) const override;
    virtual bool contains(SubsystemOutputMapInterface::Item * const item) const override;
    virtual bool contains(SubsystemOutputMapInterface::Index const & index) const override;

public:
    virtual SubsystemOutputMapInterface::Iterator end() override;
    virtual SubsystemOutputMapInterface::ConstIterator end() const override;

public:
    virtual bool setID(SubsystemOutputMapInterface::Item * const item,
                       SubsystemOutputMapInterface::ID const & id) override;
    virtual SubsystemOutputMapInterface::ID generateID() const override;
    virtual SubsystemOutputMapInterface::ID getID(SubsystemOutputMapInterface::Item * const item) const override;
    virtual SubsystemOutputMapInterface::ID getID(SubsystemOutputMapInterface::Index const & index) const override;

public:
    virtual SubsystemOutputMapInterface::Index getIndex(SubsystemOutputMapInterface::ID const & id) const override;
    virtual SubsystemOutputMapInterface::Index getIndex(SubsystemOutputMapInterface::Item * const item) const override;

public:
    virtual SubsystemOutputMapInterface::Item * getItem(SubsystemOutputMapInterface::ID const & id) const override;
    virtual SubsystemOutputMapInterface::Item * getItem(SubsystemOutputMapInterface::Index const & index) const override;

public:
    virtual bool remove(SubsystemOutputMapInterface::ID const & id) override;
    virtual bool remove(SubsystemOutputMapInterface::Item * const item) override;
    virtual bool remove(SubsystemOutputMapInterface::Index const & index) override;

protected:
    SubsystemOutputMapInterface::Map map;                                       //!< The map of subsystem ouptut items
    SubsystemOutputMapInterface::ComponentOutputs * const componentOutputs;     //!< The pointer to the component output map (initialized to the argument passed to the constructor)
};

#endif // SUBSYSTEMOUTPUTMAPMODEL_H
