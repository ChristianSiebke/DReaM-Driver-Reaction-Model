#ifndef PARAMETERITEMVIEW_H
#define PARAMETERITEMVIEW_H

#include <QWidget>

namespace Ui {
class ParameterItemView;
}

class ParameterItemView : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterItemView(QString key,
                               QString type,
                               QString value,
                               QString unit,
                               QWidget *parent = 0);
    ~ParameterItemView();

Q_SIGNALS:
    void valueChanged(QString const &value);

private Q_SLOTS:
    void toggled(bool checked);

private:
    int adjustStepSize(double value);

private:
    Ui::ParameterItemView *ui;
};

#endif // PARAMETERITEMVIEW_H
