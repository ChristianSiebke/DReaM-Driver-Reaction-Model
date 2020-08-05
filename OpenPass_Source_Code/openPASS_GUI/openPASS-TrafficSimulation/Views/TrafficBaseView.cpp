#include "TrafficBaseView.h"

#include <iostream>

#include <QPainter>

TrafficBaseView::TrafficBaseView(QWidget *parent) :
    WidgetView(parent)
{
    trafficView = new TrafficView(this);
    trafficLanesView = new TrafficLanesView(this);
    trafficParametersView = new TrafficParametersView(this);

    connect(trafficParametersView, &TrafficParametersView::resized, this, &TrafficBaseView::updateSize);
    connect(trafficLanesView, &TrafficLanesView::resized, this, &TrafficBaseView::updateSize);

  //  adjustWidgets(parent);
}



TrafficView * TrafficBaseView::getTrafficView() const
{
    return trafficView;
}

TrafficParametersView * TrafficBaseView::getTrafficParametersView() const
{
    return trafficParametersView;
}

TrafficLanesView * TrafficBaseView::getTrafficLanesView() const
{
    return trafficLanesView;
}

void TrafficBaseView::updateSize()
{
    QRect frame(0,0,1,1);

    for(auto child : findChildren<WidgetView*>())
        frame = frame.united(child->geometry());

    setGeometry(frame);
}

void TrafficBaseView::adjustWidgets(QWidget *mainWindow)
{
    int x,y;

    // adjust the geometry of the Traffic View:
    x= (mainWindow->width() - trafficView->width() )/2;
    trafficView->move(x,30);
  //  trafficView->setGeometry(x,30,mainWindow->width()/3,trafficView->height());

    // adjust the geometry of Traffic Parameters View:
    x = trafficView->x() - trafficParametersView->width()/2 - 30;
    y = trafficView->height() + 130;
    trafficParametersView->move(x,y);
  //  trafficParametersView->setGeometry(x,y, mainWindow->width()/3, trafficParametersView->height());

    // adjust the geometry of Traffic Lanes View:
    x = trafficView->x() + trafficView->width() - trafficLanesView->width()/2 + 30;
    y = trafficView->height() + 130;
    trafficLanesView->move(x,y);
  //  trafficLanesView->setGeometry(x,y, mainWindow->width()/3, trafficLanesView->height());

    updateSize();

}

void TrafficBaseView::paintEvent(QPaintEvent *event)
{
    event->ignore();
    QPainter painter(this);

    QPoint P1(trafficView->x() + trafficView->width()/2, trafficView->y() + trafficView->height()) ;
    QPoint P2(trafficParametersView->x() + trafficParametersView->width()/2, trafficParametersView->y());
    QPoint P3(trafficLanesView->x() + trafficLanesView->width()/2, trafficLanesView->y());

    painter.setPen(QColor(QStringLiteral("#777777")));
    QPainterPath path(P1);

    path.lineTo(P2);
    painter.drawPath(path);

    path.lineTo(P1);

    path.lineTo(P3);
    painter.drawPath(path);
}
