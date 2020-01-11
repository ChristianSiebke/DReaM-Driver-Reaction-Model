#ifndef SYSTEMCOMPONENTPARAMETERVALUENORMALDISTRIBUTIONVIEW_H
#define SYSTEMCOMPONENTPARAMETERVALUENORMALDISTRIBUTIONVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>

class SystemComponentParameterItemInterface;

class SystemComponentParameterValueNormalDistributionView : public QWidget
{
    Q_OBJECT
public:
    explicit SystemComponentParameterValueNormalDistributionView(SystemComponentParameterItemInterface* const parameter,
                                                                 QWidget *parent = nullptr);
    virtual ~SystemComponentParameterValueNormalDistributionView() = default;

public Q_SLOTS:
    void updateInterface();
    void updateView();

protected:
    SystemComponentParameterItemInterface * const parameter;

private:
    QDoubleSpinBox * const meanView;
    QDoubleSpinBox * const sdView;
    QDoubleSpinBox * const minView;
    QDoubleSpinBox * const maxView;
};

#endif // SYSTEMCOMPONENTPARAMETERVALUENORMALDISTRIBUTIONVIEW_H
