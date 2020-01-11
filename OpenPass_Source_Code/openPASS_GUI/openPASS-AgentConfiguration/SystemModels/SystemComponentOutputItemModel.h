#ifndef SYSTEMCOMPONENTOUTPUTITEMMODEL_H
#define SYSTEMCOMPONENTOUTPUTITEMMODEL_H

#include "openPASS-System/SystemComponentOutputItemInterface.h"

class SystemComponentOutputItemModel : public SystemComponentOutputItemInterface
{
    Q_OBJECT

public:
    explicit SystemComponentOutputItemModel(ComponentOutputItemInterface const * const output,
                                            QObject * const parent = nullptr);
    virtual ~SystemComponentOutputItemModel() = default;

public:
    virtual SystemComponentItemInterface * getComponent() const override;

public:
    virtual SystemComponentOutputItemInterface::ID getID() const override;
    virtual SystemComponentOutputItemInterface::Type getType() const override;
    virtual SystemComponentOutputItemInterface::Title getTitle() const override;
    virtual SystemComponentOutputItemInterface::Unit getUnit() const override;

public Q_SLOTS:
    void modifyTitle();
    void modifyType();
    void modifyUnit();

protected:
    ComponentOutputItemInterface const * const output;

protected:
    ComponentOutputItemInterface::Type type;
    ComponentOutputItemInterface::Title title;
    ComponentOutputItemInterface::Unit unit;
};

#endif // SYSTEMCOMPONENTOUTPUTITEMMODEL_H
