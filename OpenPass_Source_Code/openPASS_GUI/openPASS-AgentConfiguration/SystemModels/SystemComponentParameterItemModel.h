#ifndef SYSTEMCOMPONENTPARAMETERITEMMODEL_H
#define SYSTEMCOMPONENTPARAMETERITEMMODEL_H

#include "openPASS-System/SystemComponentParameterItemInterface.h"

class SystemComponentParameterItemModel : public SystemComponentParameterItemInterface
{
    Q_OBJECT

public:
    explicit SystemComponentParameterItemModel(ComponentParameterItemInterface const * const parameter,
                                               QObject * const parent = nullptr);
    virtual ~SystemComponentParameterItemModel() = default;

public:
    virtual SystemComponentParameterItemInterface::ID getID() const override;
    virtual SystemComponentParameterItemInterface::Type getType() const override;
    virtual SystemComponentParameterItemInterface::Title getTitle() const override;
    virtual SystemComponentParameterItemInterface::Unit getUnit() const override;

public:
    virtual bool setValue(SystemComponentParameterItemInterface::Value const & value) override;
    virtual SystemComponentParameterItemInterface::Value getValue() const override;

public:
    virtual SystemComponentItemInterface * getComponent() const override;

protected:
    ComponentParameterItemInterface const * const parameter;

private Q_SLOTS:
    void modifyTitle();

protected:
    SystemComponentParameterItemInterface::Type type;
    SystemComponentParameterItemInterface::Title title;
    SystemComponentParameterItemInterface::Unit unit;
    SystemComponentParameterItemInterface::Value value;
};

#endif // SYSTEMCOMPONENTPARAMETERITEMMODEL_H
