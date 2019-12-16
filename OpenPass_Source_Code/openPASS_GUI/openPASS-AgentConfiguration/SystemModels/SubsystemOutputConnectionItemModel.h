//-----------------------------------------------------------------------------
//! @file  SubsystemOutputConnectionItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemOutputConnectionItemInterface
//!        class. It contains pointers to the source (SystemComponentOutputItemInterface) and target (SubsystemOutputItemInterface)
//!        items of the output connection.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMOUTPUTCONNECTIONITEMMODEL_H
#define SUBSYSTEMOUTPUTCONNECTIONITEMMODEL_H

#include "openPASS-System/SubsystemOutputConnectionItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemOutputConnectionItemInterface
//!        class. It contains pointers to the source (SystemComponentOutputItemInterface) and target (SubsystemOutputItemInterface)
//!        items of the output connection.
//-----------------------------------------------------------------------------
class SubsystemOutputConnectionItemModel : public SubsystemOutputConnectionItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! This constructor initializes the source and target pointers to the values
    //! passed as first and second arguments. It also sets the parent of this output connection
    //! if passed as a third argument (usually an instance of SubsystemOutputConnectionMapModel)
    //!
    //! @param[in]      source      The source from which this connection should start (refers
    //!                             to an instance of SystemComponentOutputItemInterface)
    //! @param[in]      target      The target in which this connection should end (refers to
    //!                             an instance of SubsystemOutputItemInterface)
    //! @param[in]      parent      The parent object
    //-----------------------------------------------------------------------------
    explicit SubsystemOutputConnectionItemModel(SubsystemOutputConnectionItemInterface::Source const * const source,
                                       SubsystemOutputConnectionItemInterface::Target const * const target,
                                       QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemOutputConnectionItemModel() = default;

public:
    virtual SubsystemOutputConnectionItemInterface::ID getID() const override;
    virtual SubsystemOutputConnectionItemInterface::Source const * getSource() const override;
    virtual SubsystemOutputConnectionItemInterface::Target const * getTarget() const override;
    virtual bool isCompatible() const override;

public:
    //-----------------------------------------------------------------------------
    //! Checks whether the given source and target items are compatible, i.e.
    //! whether they have the same data type and unit. If no unit is given, the item
    //! is compatible to any other item with the same data type.
    //!
    //! @param[in]      source      a source (instance of SystemComponentOutputItemInterface)
    //! @param[in]      target      a target (instance of SubsystemOutputItemInterface)
    //!
    //! @return                     "true" if source and target are compatible, "false" otherwise
    //-----------------------------------------------------------------------------
    static bool isCompatible(SubsystemOutputConnectionItemInterface::Source const * const source,
                             SubsystemOutputConnectionItemInterface::Target const * const target);


protected:
    SubsystemOutputConnectionItemInterface::Source const * const source;    //!< The pointer to the system component output item from which this connection starts
    SubsystemOutputConnectionItemInterface::Target const * const target;    //!< The pointer to the subsystem output in which this connection ends

};

#endif // SUBSYSTEMOUTPUTCONNECTIONITEMMODEL_H
