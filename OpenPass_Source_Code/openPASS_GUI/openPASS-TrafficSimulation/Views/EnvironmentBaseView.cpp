#include "EnvironmentBaseView.h"

EnvironmentBaseView::EnvironmentBaseView(QWidget *parent)
    : WidgetView(parent)
{

    environmentView = new EnvironmentView(this);

    connect(environmentView, &EnvironmentView::resized, this, &EnvironmentBaseView::updateSize);

    adjustWidgets(parent);
}

EnvironmentView * EnvironmentBaseView::getEnvironmentView() const
{
    return environmentView;
}

void EnvironmentBaseView::updateSize()
{
    QRect frame(0,0,1,1);

    for(auto child : findChildren<WidgetView*>())
        frame = frame.united(child->geometry());

    setGeometry(frame);
}

void EnvironmentBaseView::adjustWidgets(QWidget *mainWindow)
{
    int x;

    // adjust the position of Environment View:
    x= (mainWindow->width() - environmentView->width() )/2;
    environmentView->move(x,30);

    updateSize();
}
