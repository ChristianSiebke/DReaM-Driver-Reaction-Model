//-----------------------------------------------------------------------------
//! @file  SubsystemParameterMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemParameterMapInterface
//!        class. It contains a reference to the subsystem parameter map as well as to the component
//!        parameter map (ComponentParameterMapInterface). The latter is needed to create component parameters
//!        whenever subsystem parameters are created.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMPARAMETERMAPMODEL_H
#define SUBSYSTEMPARAMETERMAPMODEL_H

#include "openPASS-System/SubsystemParameterMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemParameterMapInterface
//!        class. It contains a reference to the subsystem parameter map as well as to the component
//!        parameter map (ComponentParameterMapInterface). The latter is needed to create component parameters
//!        whenever subsystem parameters are created.
//-----------------------------------------------------------------------------
class SubsystemParameterMapModel : public SubsystemParameterMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Initializes the pointer to the component parameter map such that corresponding component parameters can
    //! be created whenever subsystem parameters are created. Sets the QObject as a parent if passed
    //! to the constructor as a second argument.
    //!
    //! @param[in]      componentParameters     The pointer to the component parameter map
    //! @param[in]      parent                  The pointer to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemParameterMapModel(ComponentParameterMapInterface * const componentParameters,
                                        QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemParameterMapModel() = default;

public:
    virtual bool add(SubsystemParameterMapInterface::ID const & id,
                     SubsystemParameterMapInterface::SystemComponentParameter * const systemComponentParameter) override;

    virtual bool add(SubsystemParameterItemInterface::ComponentParameterItem * const componentParameterItem,
                                         SubsystemParameterMapInterface::SystemComponentParameter * const systemComponentParameterItem) override;

    virtual bool add(SubsystemParameterMapInterface::ID const & id,
                     SubsystemParameterMapInterface::Item * const item) override;

public:
    virtual SubsystemParameterMapInterface::Iterator begin() override;
    virtual SubsystemParameterMapInterface::ConstIterator begin() const override;

public:
    virtual bool clear() override;

public:
    virtual int count() const override;

public:
    virtual bool contains(SubsystemParameterMapInterface::ID const & id) const override;
    virtual bool contains(SubsystemParameterMapInterface::Item * const item) const override;
    virtual bool contains(SubsystemParameterMapInterface::Index const & index) const override;

public:
    virtual SubsystemParameterMapInterface::Iterator end() override;
    virtual SubsystemParameterMapInterface::ConstIterator end() const override;

public:
    virtual bool setID(SubsystemParameterMapInterface::Item * const item,
                       SubsystemParameterMapInterface::ID const & id) override;
    virtual SubsystemParameterMapInterface::ID generateID() const override;
    virtual SubsystemParameterMapInterface::ID getID(SubsystemParameterMapInterface::Item * const item) const override;
    virtual SubsystemParameterMapInterface::ID getID(SubsystemParameterMapInterface::Index const & index) const override;

public:
    virtual SubsystemParameterMapInterface::Index getIndex(SubsystemParameterMapInterface::ID const & id) const override;
    virtual SubsystemParameterMapInterface::Index getIndex(SubsystemParameterMapInterface::Item * const item) const override;

public:
    virtual SubsystemParameterMapInterface::Item * getItem(SubsystemParameterMapInterface::ID const & id) const override;
    virtual SubsystemParameterMapInterface::Item * getItem(SubsystemParameterMapInterface::Index const & index) const override;

public:
    virtual bool remove(SubsystemParameterMapInterface::ID const & id) override;
    virtual bool remove(SubsystemParameterMapInterface::Item * const item) override;
    virtual bool remove(SubsystemParameterMapInterface::Index const & index) override;

protected:
    SubsystemParameterMapInterface::Map map;                                            //!< The map of subsystem parameter items
    SubsystemParameterMapInterface::ComponentParameters * const componentParameters;    //!< The pointer to the component parameter map (initialized to the argument passed to the constructor)
};

#endif // SUBSYSTEMPARAMETERMAPMODEL_H
