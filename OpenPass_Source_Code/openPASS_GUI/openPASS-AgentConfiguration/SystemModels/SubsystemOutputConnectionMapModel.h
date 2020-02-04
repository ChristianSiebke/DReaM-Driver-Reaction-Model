//-----------------------------------------------------------------------------
//! @file  SubsystemOutputConnectionMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemOutputConnectionMapInterface
//!        class. It contains the map of output connections and some additional slots for the deletion
//!        of output connections when contributing items are deleted (see below).
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMOUTPUTCONNECTIONMAPMODEL_H
#define SUBSYSTEMOUTPUTCONNECTIONMAPMODEL_H

#include "openPASS-System/SubsystemOutputConnectionMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemOutputConnectionMapInterface
//!        class. It contains the map of output connections and some additional slots for the deletion
//!        of output connections when contributing items are deleted (see below).
//-----------------------------------------------------------------------------
class SubsystemOutputConnectionMapModel : public SubsystemOutputConnectionMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Ordinary constructor, which sets the parent if passed as an argument.
    //!
    //! @param[in]      parent      The pointer to the parent object
    //-----------------------------------------------------------------------------
    explicit SubsystemOutputConnectionMapModel(QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemOutputConnectionMapModel() = default;

public:
    virtual SubsystemOutputConnectionMapInterface::Iterator begin() override;
    virtual SubsystemOutputConnectionMapInterface::ConstIterator begin() const override;

public:
    virtual SubsystemOutputConnectionMapInterface::Iterator end() override;
    virtual SubsystemOutputConnectionMapInterface::ConstIterator end() const override;

public:
    virtual SubsystemOutputConnectionMapInterface::ID getID(SubsystemOutputConnectionMapInterface::Item * const item) const override;
    virtual SubsystemOutputConnectionMapInterface::Item * getItem(SubsystemOutputConnectionMapInterface::ID const & id) const override;

public:
    virtual bool clear() override;
    virtual bool add(SubsystemOutputConnectionMapInterface::ID const & id,
                     SubsystemOutputConnectionMapInterface::Source const * const source,
                     SubsystemOutputConnectionMapInterface::Target const * const target) override;
    virtual bool add(SubsystemOutputConnectionMapInterface::Source const * const source,
                     SubsystemOutputConnectionMapInterface::Target const * const target) override;
    virtual bool contains(SubsystemOutputConnectionMapInterface::Source const * const source,
                          SubsystemOutputConnectionMapInterface::Target const * const target) const override;
    virtual bool remove(SubsystemOutputConnectionMapInterface::Component const * const component) override;
    virtual bool remove(SubsystemOutputConnectionMapInterface::Source const * const source) override;
    virtual bool remove(SubsystemOutputConnectionMapInterface::Target const * const target) override;
    virtual bool remove(SubsystemOutputConnectionMapInterface::Item * connection) override;
    virtual QList<Item*> values() override;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! This slot calls the SubsystemOutputConnectionMapModel::remove(Source const * const source)
    //! method. It will be triggered, before the given source item is going to be deleted.
    //!
    //! @param[in]      source      a source item (= system component output)
    //-----------------------------------------------------------------------------
    void removeSourceConnection(SubsystemOutputConnectionMapInterface::Source const * const source);

    //-----------------------------------------------------------------------------
    //! This slot calls the SubsystemOutputConnectionMapModel::remove(Target const * const target)
    //! method. It will be triggered, before the given target item is going to be deleted.
    //!
    //! @param[in]      target      a target item (= subsystem output)
    //-----------------------------------------------------------------------------
    void removeTargetConnection(SubsystemOutputConnectionMapInterface::Target const * const target);

    //-----------------------------------------------------------------------------
    //! This slot calls the SubsystemOutputConnectionMapModel::remove(Component const * const component)
    //! method. It will be triggered, before the given component item is going to be deleted.
    //!
    //! @param[in]      component    a system component item
    //-----------------------------------------------------------------------------
    void removeComponentConnection(SubsystemOutputConnectionMapInterface::Component const * const component);

protected:
    //-----------------------------------------------------------------------------
    //! This function returns the next unused key identifier (ID) of the output connections map.
    //!
    //! @return                      an unused ID which is one greater than the largest used ID
    //-----------------------------------------------------------------------------
    virtual SubsystemOutputConnectionMapInterface::ID next() const;

    //-----------------------------------------------------------------------------
    //! This method removes the output connections with the key identifiers (IDs)
    //! in the submitted list. If the list is not empty, the SubsystemOutputConnectionMapInterface::modified
    //! signal will be emitted in order to trigger an update of the views.
    //! If an ID does not correspond to an output connection, nothing will be done for that list entry.
    //!
    //! @param[in]      ids         a list of output connection IDs (QList)
    //!
    //! @return                     so far, always "true" (could be made a void function)
    //-----------------------------------------------------------------------------
    virtual bool remove(QList<SubsystemOutputConnectionMapInterface::ID> const & ids);

protected:
    SubsystemOutputConnectionMapInterface::Map connections;     //!< The map of output connections
};

#endif // SUBSYSTEMOUTPUTCONNECTIONMAPMODEL_H
