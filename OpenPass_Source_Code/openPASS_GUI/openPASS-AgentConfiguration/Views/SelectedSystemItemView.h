#ifndef SELECTEDSYSTEMITEMVIEW_H
#define SELECTEDSYSTEMITEMVIEW_H

#include "Views/WidgetView.h"

#include <QString>

namespace Ui {
class SelectedSystemItemView;
}

class SelectedSystemItemView : public WidgetView
{
    Q_OBJECT

public:
    explicit SelectedSystemItemView(double probability, QWidget *parent = 0);
    ~SelectedSystemItemView();

Q_SIGNALS:
    void removeSystem(QString const &systemTitle);
    void modifiedProbability(QString const &systemTitle, double value);

public Q_SLOTS:
    void on_remove_clicked();
    void on_probability_editingFinished();

public:
    void setSystem(QString const &name);
    QString getSystem() const;

public:
    unsigned int getIndex() const ;
    void setIndex(unsigned int _index);

private:
    unsigned int index;
    Ui::SelectedSystemItemView *ui;
};

#endif // SELECTEDSYSTEMITEMVIEW_H
