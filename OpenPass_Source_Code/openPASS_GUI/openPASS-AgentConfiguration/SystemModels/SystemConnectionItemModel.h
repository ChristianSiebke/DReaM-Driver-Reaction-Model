#ifndef SYSTEMCONNECTIONITEMMODEL_H
#define SYSTEMCONNECTIONITEMMODEL_H

#include "openPASS-System/SystemConnectionItemInterface.h"

class SystemConnectionItemModel : public SystemConnectionItemInterface
{
    Q_OBJECT

public:
    explicit SystemConnectionItemModel(SystemConnectionItemInterface::Source const * const source,
                                       SystemConnectionItemInterface::Target const * const target,
                                       QObject * const parent = nullptr);
    virtual ~SystemConnectionItemModel() = default;

public:
    virtual SystemConnectionItemInterface::ID getID() const override;
    virtual SystemConnectionItemInterface::Source const * getSource() const override;
    virtual SystemConnectionItemInterface::Target const * getTarget() const override;
    virtual bool isCompatible() const override;

public:
    static bool isCompatible(SystemConnectionItemInterface::Source const * const source,
                             SystemConnectionItemInterface::Target const * const target);

protected:
    SystemConnectionItemInterface::Source const * const source;
    SystemConnectionItemInterface::Target const * const target;
};

#endif // SYSTEMCONNECTIONITEMMODEL_H
