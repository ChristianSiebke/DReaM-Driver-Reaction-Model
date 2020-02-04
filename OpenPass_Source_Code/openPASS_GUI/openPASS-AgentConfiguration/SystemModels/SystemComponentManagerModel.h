#ifndef SYSTEMCOMPONENTMANAGERMODEL_H
#define SYSTEMCOMPONENTMANAGERMODEL_H

#include "openPASS-Component/ComponentInterface.h"
#include "openPASS-Component/ComponentMapInterface.h"
#include "openPASS-System/SystemComponentManagerInterface.h"

class SystemComponentManagerModel : public SystemComponentManagerInterface
{
    Q_OBJECT

public:
    explicit SystemComponentManagerModel(ComponentInterface * const component,
                                         QObject * const parent = nullptr);
    virtual ~SystemComponentManagerModel() = default;

public:
    virtual bool loadFromDirectory(QDir const & directory) override;
    virtual QList<ComponentItemInterface::Title> listTitlesByType(ComponentItemInterface::Type const & type) const override;
    virtual ComponentItemInterface * lookupItemByName(ComponentItemInterface::Name const & name) const override;
    virtual ComponentItemInterface * lookupItemByTitle(ComponentItemInterface::Title const & title) const override;

public:
    virtual ComponentMapInterface * getComponents() const override;


protected:
    ComponentMapInterface * const components;
};

#endif // SYSTEMCOMPONENTMANAGERMODEL_H
