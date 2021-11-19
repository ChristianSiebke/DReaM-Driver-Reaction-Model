/********************************************************************************
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#ifndef PROFILEITEMVIEW_H
#define PROFILEITEMVIEW_H

#include "WidgetView.h"

namespace Ui {
class ProfileItemView;
}

class ProfileItemView : public WidgetView
{
    Q_OBJECT

public:
    explicit ProfileItemView(const QString &label,
                             QString const &name,
                             double probability,
                             QWidget *parent = 0);
    ~ProfileItemView();

Q_SIGNALS:
    void probabilityChanged(QString const &name, double probability);
    void remove(QString const &name);

private Q_SLOTS:
    void on_probability_editingFinished();
    void on_remove_clicked();

public:
    void setLabel(QString const &label);
    void setName(QString const &_name);

private:
    QString name;
    Ui::ProfileItemView *ui;
};

#endif // PROFILEITEMVIEW_H
