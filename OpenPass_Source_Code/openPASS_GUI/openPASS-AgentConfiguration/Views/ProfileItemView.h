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
