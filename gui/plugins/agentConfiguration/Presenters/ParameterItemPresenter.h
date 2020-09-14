#ifndef PARAMETERITEMPRESENTER_H
#define PARAMETERITEMPRESENTER_H

#include <QObject>

#include "Views/ParameterItemView.h"
#include "openPASS-AgentConfiguration/ParameterItemInterface.h"

class ParameterItemPresenter : public QObject
{
    Q_OBJECT

public:
    explicit ParameterItemPresenter(ParameterItemInterface *parameter,
                                    ParameterItemView *parameterView,
                                    QObject *parent = 0);

private Q_SLOTS:
    void setValue(QString const &value);

private:
    ParameterItemInterface *parameter;
    ParameterItemView *parameterView;
};

#endif // PARAMETERITEMPRESENTER_H
