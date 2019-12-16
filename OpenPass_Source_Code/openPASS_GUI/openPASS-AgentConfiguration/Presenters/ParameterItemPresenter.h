#ifndef PARAMETERITEMPRESENTER_H
#define PARAMETERITEMPRESENTER_H

#include "openPASS-AgentConfiguration/ParameterItemInterface.h"
#include "Views/ParameterItemView.h"

#include <QObject>

class ParameterItemPresenter : public QObject
{
    Q_OBJECT

public:
    explicit ParameterItemPresenter(ParameterItemInterface * parameter,
                                    ParameterItemView * parameterView,
                                    QObject *parent = 0);

private Q_SLOTS:
    void setValue(QString const & value);

private:
    ParameterItemInterface * parameter;
    ParameterItemView * parameterView;
};

#endif // PARAMETERITEMPRESENTER_H
