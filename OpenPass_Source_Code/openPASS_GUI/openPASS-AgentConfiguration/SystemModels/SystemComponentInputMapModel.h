#ifndef SYSTEMCOMPONENTINPUTMAPMODEL_H
#define SYSTEMCOMPONENTINPUTMAPMODEL_H

#include "openPASS-Component/ComponentInputMapInterface.h"
#include "openPASS-System/SystemComponentInputMapInterface.h"

class SystemComponentInputMapModel : public SystemComponentInputMapInterface
{
    Q_OBJECT

public:
    explicit SystemComponentInputMapModel(ComponentInputMapInterface const * const inputs,
                                          QObject * const parent = nullptr);
    virtual ~SystemComponentInputMapModel() = default;

public:
    virtual SystemComponentInputMapInterface::Iterator begin() override;
    virtual SystemComponentInputMapInterface::ConstIterator begin() const override;

public:
    virtual bool contains(SystemComponentInputMapInterface::ID const & id) const override;

public:
    virtual SystemComponentInputMapInterface::Iterator end() override;
    virtual SystemComponentInputMapInterface::ConstIterator end() const override;

public:
    virtual SystemComponentInputMapInterface::ID getID(SystemComponentInputMapInterface::Item * const item) const override;
    virtual SystemComponentInputMapInterface::Item * getItem(SystemComponentInputMapInterface::ID const & id) const override;

public Q_SLOTS:
    void  add( ComponentInputItemInterface const * const componentInput);
    void  remove(const ComponentInputMapInterface::Item * const componentInput);

protected:
    SystemComponentInputMapInterface::Map inputs;
};

#endif // SYSTEMCOMPONENTINPUTMAPMODEL_H
