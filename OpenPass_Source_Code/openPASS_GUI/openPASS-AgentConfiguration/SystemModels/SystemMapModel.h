//-----------------------------------------------------------------------------
//! @file  SystemMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SystemMapInterface
//!         class
//-----------------------------------------------------------------------------

#ifndef SYSTEMMAPMODEL_H
#define SYSTEMMAPMODEL_H

#include "openPASS-System/SystemMapInterface.h"

class SystemComponentManagerInterface;

/**
 * @brief This model class implements the functionality of the SystemMapInterface class.
 **/
class SystemMapModel : public SystemMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! A constructor, which just sets the parent if passed to it.
    //!
    //! @param[in]      parent          The reference to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SystemMapModel(QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SystemMapModel() = default;

public:
    virtual SystemMapInterface::Iterator begin() override;
    virtual SystemMapInterface::ConstIterator begin() const override;

public:
    virtual SystemMapInterface::Iterator end() override;
    virtual SystemMapInterface::ConstIterator end() const override;

public:
    virtual SystemMapInterface::ID getID(SystemMapInterface::Item * const item) const override;
    virtual SystemMapInterface::Item * getItem(SystemMapInterface::ID const & id) const override;
    virtual SystemMapInterface::ID generateID() override;
    virtual SystemItemInterface::Title generateTitle() override;

public:
    virtual bool add(SystemMapInterface::ID const &id) override;
    virtual bool add(SystemMapInterface::ID const & id,
                     SystemMapInterface::Item * const system) override;

public:
    virtual QList <SystemMapInterface::Item *> values() override;
    virtual int count() const override;

public:
    virtual bool contains(SystemItemInterface::Title title) override;
    virtual bool contains(SystemItemInterface::ID const & id) override;

public:
    virtual SystemMapInterface::ID lastID() override;


public:
    virtual bool remove(SystemMapInterface::ID const & id) override;
    virtual bool clear() override;


protected:
    SystemMapInterface::Map systems;            //!< The map of systems
};

#endif // SYSTEMMAPMODEL_H
