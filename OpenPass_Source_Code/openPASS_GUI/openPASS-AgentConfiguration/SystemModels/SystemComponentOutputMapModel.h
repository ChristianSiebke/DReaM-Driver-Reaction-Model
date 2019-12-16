#ifndef SYSTEMCOMPONENTOUTPUTMAPMODEL_H
#define SYSTEMCOMPONENTOUTPUTMAPMODEL_H

#include "openPASS-Component/ComponentOutputMapInterface.h"
#include "openPASS-System/SystemComponentOutputMapInterface.h"

class SystemComponentOutputMapModel : public SystemComponentOutputMapInterface
{
    Q_OBJECT

public:
    explicit SystemComponentOutputMapModel(ComponentOutputMapInterface const * const outputs,
                                           QObject * const parent = nullptr);
    virtual ~SystemComponentOutputMapModel() = default;

public:
    virtual SystemComponentOutputMapInterface::Iterator begin() override;
    virtual SystemComponentOutputMapInterface::ConstIterator begin() const override;

public:
    virtual bool contains(SystemComponentOutputMapInterface::ID const & id) const override;

public:
    virtual SystemComponentOutputMapInterface::Iterator end() override;
    virtual SystemComponentOutputMapInterface::ConstIterator end() const override;

public:
    virtual SystemComponentOutputMapInterface::ID getID(SystemComponentOutputMapInterface::Item * const item) const override;
    virtual SystemComponentOutputMapInterface::Item * getItem(SystemComponentOutputMapInterface::ID const & id) const override;

public Q_SLOTS:
    void  add( ComponentOutputItemInterface const * const componentOutput);
    void  remove(const ComponentOutputMapInterface::Item * const componentOutput);

protected:
    SystemComponentOutputMapInterface::Map outputs;
};

#endif // SYSTEMCOMPONENTOUTPUTMAPMODEL_H
