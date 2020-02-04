//-----------------------------------------------------------------------------
//! @file  SystemModel.h
//! @ingroup systemPlugin
//! @brief This model class implements the functionality of the SystemInterface class.
//!
//-----------------------------------------------------------------------------

#ifndef SYSTEMMODEL_H
#define SYSTEMMODEL_H

#include "openPASS-System/SystemInterface.h"

class ComponentInterface;
class ProjectInterface;
class SystemComponentManagerModel;

/**
 * @brief This model class implements the functionality of the SystemInterface class.
 **/
class SystemModel : public SystemInterface
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! The construction of a SystemModel object (to be referenced by SystemInterface)
    //! is based on ComponentInterface and ProjectInterface objects. From the ComponentInterface
    //! object, the pointer to the component manager "components" (SystemComponentManagerInterface) is initialized.
    //! The ProjectInterface argument is used to initialize the ProjectInterface member variable "_project", which allows for
    //! accessing the file system. In the initializer, the system and subsystem maps are created, using this object as
    //! a parent. In the body of the constructor, the component manager loads the component XML from the library
    //! specified by _project. Finally, the signal SubsystemMapInterface::removed is connected to the slot
    //! removeSubsystemComponentItems() in order to synchronize the deletion of a subsystem (closing its tab) with the
    //! deletion of all occurences of the subsystem as system component items (SystemComponentItemInterface).
    //!
    //! @param[in]      component       The ComponentInterface object
    //! @param[in]      project         The ProjectInterface object
    //! @param[in]      parent          A QObject item which can be used as a parent
    //-----------------------------------------------------------------------------
    explicit SystemModel(ComponentInterface * const component,
                         ProjectInterface * const project,
                         QObject * const parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Default desctructor
    //-----------------------------------------------------------------------------
    virtual ~SystemModel() = default;

public:
    virtual Filepath getCurrentFilepath() const override;
    virtual void setCurrentFilepath(Filepath const &_filepath) override;

public:
    virtual bool clear() override;
    virtual bool load(QString const & filepath) override;
    virtual bool save(QString const & filepath) const override;

public:
    virtual SystemComponentManagerInterface * getComponents() const override;
    virtual SystemMapInterface * getSystems() const override;
    virtual SubsystemMapInterface *getSubsystems() const override;


private Q_SLOTS:
    //-----------------------------------------------------------------------------
    //! This slot is called when a subsystem (SubsystemItemInterface) is deleted, i.e.
    //! a subsystem tab is closed on the GUI. Calling this slot will remove all occurences of
    //! individual system component items (SystemComponentItemInterface) that are based on
    //! the subsystem to be deleted.
    //!
    //! @param[in]      library         The library of the system component item to be removed
    //!                                 (in fact, this will correspond to a subsystem's title)
    //-----------------------------------------------------------------------------
    void removeSubsystemComponentItems(SystemComponentItemInterface::Library library);


protected:
    SystemComponentManagerInterface * const components;         //!< The pointer to the component manager for loading the components
    ProjectInterface * const _project;                          //!< The pointer to the project interface for accessing the file system
    SystemMapInterface * const systems;                         //!< The pointer to the system map
    SubsystemMapInterface * const subsystems;                   //!< The pointer to the subsystem map
    Filepath currentFilepath = Filepath("");
};

#endif // SYSTEMMODEL_H
