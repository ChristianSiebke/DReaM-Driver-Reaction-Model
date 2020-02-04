//-----------------------------------------------------------------------------
//! @file  SubsystemItemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SubsystemItemInterface
//!        class. It contains the references to the maps of connections, input and output connections, system components,
//!        subsystem inputs and outputs as well as subsystem parameters. A reference to the component item representing this
//!        subsystem as a component block is provided as well. This allows for a synchronization of all changes made here with
//!        the corresponding attributes of the component item.
//-----------------------------------------------------------------------------

#ifndef SUBSYSTEMITEMMODEL_H
#define SUBSYSTEMITEMMODEL_H

#include "openPASS-System/SubsystemItemInterface.h"
#include "openPASS-Component/ComponentItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This model class implements the functionality of the SubsystemItemInterface
//!        class. It contains the references to the maps of connections, input and output connections, system components,
//!        subsystem inputs and outputs as well as subsystem parameters. A reference to the component item representing this
//!        subsystem as a component block is provided as well. This allows for a synchronization of all changes made here with
//!        the corresponding attributes of the component item.
//-----------------------------------------------------------------------------
class SubsystemItemModel : public SubsystemItemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Initializes all attributes of this subsystem. In particular, all maps for inputs, outputs, parameters,
    //! connections and input/output connections are created. The title of the
    //! subsystem will be adopted from the name of the component item passed as the first argument.
    //! It also sets the QObject as a parent if passed as a second argument (usually an instance of
    //! SubsystemMapModel).
    //!
    //! @param[in]      component       The pointer to the component item
    //!                                 representing this subsystem
    //! @param[in]      parent          The pointer to the parent QObject
    //-----------------------------------------------------------------------------
    explicit SubsystemItemModel(ComponentItemInterface * const component,
                                QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default destructor
    //-----------------------------------------------------------------------------
    virtual ~SubsystemItemModel() = default;

public:
    virtual SubsystemItemInterface::ID getID() const override;

public:
    virtual bool setTitle(SubsystemItemInterface::Title const & title) override;
    virtual SubsystemItemInterface::Title getTitle() const override;

public:
    virtual bool setPriority(SubsystemItemInterface::Priority const & priority) override;
    virtual SubsystemItemInterface::Priority getPriority() const override;

public:
    virtual SubsystemItemInterface::ComponentMap * getComponents() const override;
    virtual SubsystemItemInterface::ConnectionMap * getConnections() const override;
    virtual SubsystemItemInterface::InputConnectionMap * getInputConnections() const override;
    virtual SubsystemItemInterface::OutputConnectionMap * getOutputConnections() const override;

public:
    virtual QList<SystemConnectionMapInterface::Source const *> getCompatibleSources(
            SystemConnectionMapInterface::Target const * const target) override;
    virtual QList<SystemConnectionMapInterface::Target const *> getCompatibleTargets(
            SystemConnectionMapInterface::Source const * const source) override;

public:
    virtual QList<SubsystemInputConnectionMapInterface::Source const *> getCompatibleInputSources(
            SubsystemInputConnectionMapInterface::Target const * const target) override;
    virtual QList<SubsystemInputConnectionMapInterface::Target const *> getCompatibleInputTargets(
            SubsystemInputConnectionMapInterface::Source const * const source) override;

public:
    virtual QList<SubsystemOutputConnectionMapInterface::Source const *> getCompatibleOutputSources(
            SubsystemOutputConnectionMapInterface::Target const * const target) override;
    virtual QList<SubsystemOutputConnectionMapInterface::Target const *> getCompatibleOutputTargets(
            SubsystemOutputConnectionMapInterface::Source const * const source) override;

public:
    virtual SubsystemItemInterface::ParameterMap * getParameters() const override;
    virtual SubsystemItemInterface::InputMap * getInputs() const override;
    virtual SubsystemItemInterface::OutputMap * getOutputs() const override;


protected:
    SubsystemItemInterface::Title title;                                        //!< The subsystem's title
    SubsystemItemInterface::Priority priority;                                  //!< The subsystem's priority (not used so far)
    SubsystemItemInterface::ConnectionMap * const connections;                  //!< The pointer to the map of connections between system components in this subsystem
    SubsystemItemInterface::InputConnectionMap * const inputconnections;        //!< The pointer to the map of connections between subsystem inputs and system components in this subsystem
    SubsystemItemInterface::OutputConnectionMap * const outputconnections;      //!< The pointer to the map of connections between subsystem outputs and system components in this subsystem

    SubsystemItemInterface::ComponentMap * const components;                    //!< The pointer to the map of system components included in this subsystem

    SubsystemItemInterface::Component * const component;                        //!< The pointer to the component item representing this subsystem (initialized to constructor argument)

    SubsystemItemInterface::InputMap * const inputs;                            //!< The pointer to the map of subsystem inputs
    SubsystemItemInterface::OutputMap * const outputs;                          //!< The pointer to the map of subsystem outputs
    SubsystemItemInterface::ParameterMap * const parameters;                    //!< The pointer to the map of subsystem parameters


};

#endif // SUBSYSTEMITEMMODEL_H
