#ifndef SYSTEMCOMPONENTITEMMODEL_H
#define SYSTEMCOMPONENTITEMMODEL_H

#include "openPASS-System/SystemComponentItemInterface.h"
#include "openPASS-System/SystemComponentManagerInterface.h"
#include "SystemComponentMapModel.h"

class SystemComponentItemModel : public SystemComponentItemInterface
{
    Q_OBJECT

public:
    explicit SystemComponentItemModel(ComponentItemInterface const * const component,
                                      SystemComponentItemInterface::Position const & position
                                      = SystemComponentItemInterface::Position(0, 0),
                                      QObject * const parent = nullptr);
    explicit SystemComponentItemModel(SystemComponentItemModel const & component,
                                      SystemComponentItemInterface::Position const & position
                                      = SystemComponentItemInterface::Position(0, 0),
                                      QObject * const parent = nullptr);
    virtual ~SystemComponentItemModel() = default;

public:
    virtual bool duplicate(SystemComponentItemInterface::Position const & position) override;
    virtual bool remove() override;

public:
    virtual SystemComponentItemInterface::ID getID() const override;
    virtual SystemComponentItemInterface::Type getType() const override;

public:
    virtual SystemComponentItemInterface::Library getLibrary() const override;

public:
    virtual bool setTitle(SystemComponentItemInterface::Title const & title) override;
    virtual SystemComponentItemInterface::Title getTitle() const override;

public:
    virtual SystemComponentItemInterface::Schedule * getSchedule() const override;
    virtual SystemComponentItemInterface::ParameterMap * getParameters() const override;
    virtual SystemComponentItemInterface::InputMap * getInputs() const override;
    virtual SystemComponentItemInterface::OutputMap * getOutputs() const override;

public:
    virtual bool setPosition(SystemComponentItemInterface::Position const & position) override;
    virtual SystemComponentItemInterface::Position getPosition() const override;

public:
    virtual SystemComponentMapModel *getComponents() const;

public Q_SLOTS:
    void modifyTitle();
    void modifyName();

protected:
    ComponentItemInterface const * const component;

protected:
    SystemComponentItemInterface::Schedule * const schedule;
    SystemComponentItemInterface::ParameterMap * const parameters;
    SystemComponentItemInterface::InputMap * const inputs;
    SystemComponentItemInterface::OutputMap * const outputs;

protected:
    SystemComponentItemInterface::Library name;
    SystemComponentItemInterface::Type type;
    SystemComponentItemInterface::Title title;
    SystemComponentItemInterface::Position position;
};

#endif // SYSTEMCOMPONENTITEMMODEL_H
