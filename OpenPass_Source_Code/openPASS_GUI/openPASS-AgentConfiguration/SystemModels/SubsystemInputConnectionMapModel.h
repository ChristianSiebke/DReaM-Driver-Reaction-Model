//-----------------------------------------------------------------------------
//! @file  SubsystemInputConnectionMapModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemInputConnectionMapInterface
//!        class. It contains the map of input connections and some additional slots for the deletion
//!        of input connections when contributing items are deleted (see below).
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMINPUTCONNECTIONMAPMODEL_H
#define SUBSYSTEMINPUTCONNECTIONMAPMODEL_H

#include "openPASS-System/SubsystemInputConnectionMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemInputConnectionMapInterface
//!        class. It contains the map of input connections and some additional slots for the deletion
//!        of input connections when contributing items are deleted (see below).
//-----------------------------------------------------------------------------
class SubsystemInputConnectionMapModel : public SubsystemInputConnectionMapInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Ordinary constructor, which sets the parent if passed as an argument.
    //!
    //! @param[in]      parent      The pointer to the parent object
    //-----------------------------------------------------------------------------
    explicit SubsystemInputConnectionMapModel(QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemInputConnectionMapModel() = default;

public:
    virtual SubsystemInputConnectionMapInterface::Iterator begin() override;
    virtual SubsystemInputConnectionMapInterface::ConstIterator begin() const override;

public:
    virtual SubsystemInputConnectionMapInterface::Iterator end() override;
    virtual SubsystemInputConnectionMapInterface::ConstIterator end() const override;

public:
    virtual SubsystemInputConnectionMapInterface::ID getID(SubsystemInputConnectionMapInterface::Item * const item) const override;
    virtual SubsystemInputConnectionMapInterface::Item * getItem(SubsystemInputConnectionMapInterface::ID const & id) const override;

public:
    virtual bool clear() override;
    virtual bool add(SubsystemInputConnectionMapInterface::ID const & id,
                     SubsystemInputConnectionMapInterface::Source const * const source,
                     SubsystemInputConnectionMapInterface::Target const * const target) override;
    virtual bool add(SubsystemInputConnectionMapInterface::Source const * const source,
                     SubsystemInputConnectionMapInterface::Target const * const target) override;
    virtual bool contains(SubsystemInputConnectionMapInterface::Source const * const source,
                          SubsystemInputConnectionMapInterface::Target const * const target) const override;
    virtual bool remove(SubsystemInputConnectionMapInterface::Component const * const component) override;
    virtual bool remove(SubsystemInputConnectionMapInterface::Source const * const source) override;
    virtual bool remove(SubsystemInputConnectionMapInterface::Target const * const target) override;
    virtual bool remove(SubsystemInputConnectionMapInterface::Item * connection) override;
    virtual QList<Item*> values() override;

public Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! This slot calls the SubsystemInputConnectionMapModel::remove(Source const * const source)
    //! method. It will be triggered, before the given source item is going to be deleted.
    //!
    //! @param[in]      source      a source item (= subsystem input)
    //-----------------------------------------------------------------------------
    void removeSourceConnection(SubsystemInputConnectionMapInterface::Source const * const source);

    //-----------------------------------------------------------------------------
    //! This slot calls the SubsystemInputConnectionMapModel::remove(Target const * const target)
    //! method. It will be triggered, before the given target item is going to be deleted.
    //!
    //! @param[in]      target      a target item (= system component input)
    //-----------------------------------------------------------------------------
    void removeTargetConnection(SubsystemInputConnectionMapInterface::Target const * const target);

    //-----------------------------------------------------------------------------
    //! This slot calls the SubsystemInputConnectionMapModel::remove(Component const * const component)
    //! method. It will be triggered, before the given component item is going to be deleted.
    //!
    //! @param[in]      component    a system component item
    //-----------------------------------------------------------------------------
    void removeComponentConnection(SubsystemInputConnectionMapInterface::Component const * const component);

protected:
    //-----------------------------------------------------------------------------
    //! This function returns the next unused key identifier (ID) of the input connections map.
    //!
    //! @return                      an unused ID which is one greater than the largest used ID
    //-----------------------------------------------------------------------------
    virtual SubsystemInputConnectionMapInterface::ID next() const;


    //-----------------------------------------------------------------------------
    //! This method removes the input connections with the key identifiers (IDs)
    //! in the submitted list. If the list is not empty, the SubsystemInputConnectionMapInterface::modified
    //! signal will be emitted in order to trigger an update of the views.
    //! If an ID does not correspond to an input connection, nothing will be done for that list entry.
    //!
    //! @param[in]      ids         a list of input connection IDs (QList)
    //!
    //! @return                     so far, always "true" (could be made a void function)
    //-----------------------------------------------------------------------------
    virtual bool remove(QList<SubsystemInputConnectionMapInterface::ID> const & ids);


protected:
    SubsystemInputConnectionMapInterface::Map connections;      //!< The map of input connections
};

#endif // SUBSYSTEMINPUTCONNECTIONMAPMODEL_H
