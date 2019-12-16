//-----------------------------------------------------------------------------
//! @file  SubsystemInputConnectionItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemInputConnectionItemInterface
//!        class. It contains pointers to the source (SubsystemInputItemInterface) and target (SystemComponentInputItemInterface)
//!        items of the input connection.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMINPUTCONNECTIONITEMMODEL_H
#define SUBSYSTEMINPUTCONNECTIONITEMMODEL_H

#include "openPASS-System/SubsystemInputConnectionItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemInputConnectionItemInterface
//!        class. It contains pointers to the source (SubsystemInputItemInterface) and target (SystemComponentInputItemInterface)
//!        items of the input connection.
//-----------------------------------------------------------------------------
class SubsystemInputConnectionItemModel : public SubsystemInputConnectionItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! This constructor initializes the source and target pointers to the values
    //! passed as first and second arguments. It also sets the parent of this input connection
    //! if passed as a third argument (usually an instance of SubsystemInputConnectionMapModel)
    //!
    //! @param[in]      source      The source from which this connection should start (refers
    //!                             to an instance of SubsystemInputItemInterface)
    //! @param[in]      target      The target in which this connection should end (refers to
    //!                             an instance of SystemComponentInputItemInterface)
    //! @param[in]      parent      The parent object
    //-----------------------------------------------------------------------------
    explicit SubsystemInputConnectionItemModel(SubsystemInputConnectionItemInterface::Source const * const source,
                                       SubsystemInputConnectionItemInterface::Target const * const target,
                                       QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemInputConnectionItemModel() = default;

public:
    virtual SubsystemInputConnectionItemInterface::ID getID() const override;
    virtual SubsystemInputConnectionItemInterface::Source const * getSource() const override;
    virtual SubsystemInputConnectionItemInterface::Target const * getTarget() const override;
    virtual bool isCompatible() const override;

public:
    //-----------------------------------------------------------------------------
    //! Checks whether the given source and target items are compatible, i.e.
    //! whether they have the same data type and unit. If no unit is given, the item
    //! is compatible to any other item with the same data type.
    //!
    //! @param[in]      source      a source (instance of SubsystemInputItemInterface)
    //! @param[in]      target      a target (instance of SystemComponentInputItemInterface)
    //!
    //! @return                     "true" if source and target are compatible, "false" otherwise
    //-----------------------------------------------------------------------------
    static bool isCompatible(SubsystemInputConnectionItemInterface::Source const * const source,
                             SubsystemInputConnectionItemInterface::Target const * const target);


protected:
    SubsystemInputConnectionItemInterface::Source const * const source;     //!< The pointer to the subsystem input item from which this connection starts
    SubsystemInputConnectionItemInterface::Target const * const target;     //!< The pointer to the system component input in which this connection ends

};

#endif // SUBSYSTEMINPUTCONNECTIONITEMMODEL_H
