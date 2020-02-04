#ifndef SYSTEMCOMPONENTINPUTITEMMODEL_H
#define SYSTEMCOMPONENTINPUTITEMMODEL_H

#include "openPASS-System/SystemComponentInputItemInterface.h"

class SystemComponentInputItemModel : public SystemComponentInputItemInterface
{
    Q_OBJECT

public:
    explicit SystemComponentInputItemModel(ComponentInputItemInterface const * const input,
                                           QObject * const parent = nullptr);
    virtual ~SystemComponentInputItemModel() = default;

public:
    virtual SystemComponentItemInterface * getComponent() const override;

public:
    virtual SystemComponentInputItemInterface::ID getID() const override;
    virtual SystemComponentInputItemInterface::Type getType() const override;
    virtual SystemComponentInputItemInterface::Title getTitle() const override;
    virtual SystemComponentInputItemInterface::Unit getUnit() const override;
    virtual SystemComponentInputItemInterface::Cardinality getCardinality() const override;

public Q_SLOTS:
    void modifyTitle();
    void modifyType();
    void modifyUnit();


protected:
    ComponentInputItemInterface const * const input;

protected:
    ComponentInputItemInterface::Type type;
    ComponentInputItemInterface::Title title;
    ComponentInputItemInterface::Unit unit;
    ComponentInputItemInterface::Cardinality cardinality;
};

#endif // SYSTEMCOMPONENTINPUTITEMMODEL_H
