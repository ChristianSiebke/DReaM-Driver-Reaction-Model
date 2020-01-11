#include "ParameterItemPresenter.h"

ParameterItemPresenter::ParameterItemPresenter(ParameterItemInterface *parameter,
                                               ParameterItemView *parameterView,
                                               QObject *parent)
    : QObject(parent)
    , parameter(parameter)
    , parameterView(parameterView)
{
    connect(parameterView, &ParameterItemView::valueChanged, this, &ParameterItemPresenter::setValue);
}

void ParameterItemPresenter::setValue(const QString &value)
{
    parameter->setValue(value);
}


