#ifndef SYSTEMCOMPONENTMAPMODEL_H
#define SYSTEMCOMPONENTMAPMODEL_H

#include "openPASS-System/SystemComponentMapInterface.h"
#include "openPASS-System/SystemComponentManagerInterface.h"

class SystemComponentItemModel;
class SystemConnectionMapInterface;

class SystemComponentMapModel : public SystemComponentMapInterface
{
    Q_OBJECT

public:
    explicit SystemComponentMapModel(SystemConnectionMapInterface * const connections,
                                     QObject * const parent = nullptr);
    virtual ~SystemComponentMapModel() = default;

public:
    virtual bool add(SystemComponentMapInterface::ID const & id,
                     SystemComponentMapInterface::Item * const component) override;
    virtual bool add(ComponentItemInterface const * const component,
                     SystemComponentItemInterface::Position const & position) override;

public:
    virtual SystemComponentMapInterface::Iterator begin() override;
    virtual SystemComponentMapInterface::ConstIterator begin() const override;

public:
    virtual bool clear() override;
    virtual bool contains(SystemComponentMapInterface::ID const &id) const override;

public:
    virtual SystemComponentMapInterface::Iterator end() override;
    virtual SystemComponentMapInterface::ConstIterator end() const override;

public:
    virtual SystemComponentMapInterface::ID getID(SystemComponentMapInterface::Item * const item) const override;
    virtual SystemComponentMapInterface::Item * getItem(SystemComponentMapInterface::ID const & id) const override;

public:
    virtual bool remove(SystemComponentMapInterface::ID const & id) override;
    virtual SystemComponentMapInterface::Item * take(SystemComponentMapInterface::ID const & id) override;

public:
    virtual bool duplicate(SystemComponentItemModel const * const component,
                           SystemComponentItemInterface::Position const & position);

    virtual QList <SystemComponentMapInterface::Item *> values() override;

    virtual ID lastID() override;

protected:
    virtual SystemComponentMapInterface::ID next() const;

protected:
 //   SystemComponentManagerInterface * const componentsManager;
    SystemConnectionMapInterface * const connections;

protected:
    SystemComponentMapInterface::Map components;
};

#endif // SYSTEMCOMPONENTMAPMODEL_H
