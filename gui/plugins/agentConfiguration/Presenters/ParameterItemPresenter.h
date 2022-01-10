/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
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
