#include "TrafficLanesView.h"

#include "TrafficItemView.h"
#include "ui_TrafficLanesView.h"

#include <QResizeEvent>

TrafficLanesView::TrafficLanesView(QWidget *parent) :
    WidgetView(parent),
    ui(new Ui::TrafficLanesView)
{
    ui->setupUi(this);
    initialHeight=height();
}

TrafficLanesView::~TrafficLanesView()
{
    delete ui;
}


void TrafficLanesView::on_addRightmostLaneAgent_clicked()
{
    Q_EMIT addRightmostLaneAgent(ui->RightmostLaneAgent->currentText());
}

void TrafficLanesView::on_addRegularLaneAgent_clicked()
{
    Q_EMIT addRegularLaneAgent(ui->RegularLaneAgent->currentText());
}

void TrafficLanesView::addRightmostLaneAgentView(unsigned int id, QString name, double probability)
{
    int index = ui->RightmostLaneLayout->count() +1;

    TrafficItemView * agentView = new TrafficItemView(TrafficItemView::Type::Agent, id, index, this);
    agentView->setValue(name);
    agentView->setProbability(probability);

    connect(agentView, &TrafficItemView::remove, this, &TrafficLanesView::removeRightmostLaneAgent);
    connect(agentView, &TrafficItemView::probabilityChanged, this, &TrafficLanesView::RightmostLaneAgent_probabilityChanged);


    ui->RightmostLaneLayout->insertWidget(index-1, agentView);
    resize(width(), height() + agentView->height());
}

void TrafficLanesView::addRegularLaneAgentView(unsigned int id, QString name, double probability)
{
    int index = ui->RegularLaneLayout->count() +1 ;
    TrafficItemView * agentView = new TrafficItemView(TrafficItemView::Type::Agent, id, index, this);
    agentView->setValue(name);
    agentView->setProbability(probability);

    connect(agentView, &TrafficItemView::remove, this, &TrafficLanesView::removeRegularLaneAgent);
    connect(agentView, &TrafficItemView::probabilityChanged, this, &TrafficLanesView::RegularLaneAgent_probabilityChanged);

    ui->RegularLaneLayout->insertWidget(index-1, agentView);
    resize(width(), height() + agentView->height());
}

void TrafficLanesView::removeRegularLaneAgentView(unsigned int id)
{
    bool shift = false;

    for(auto agentView : ui->RegularLaneWidget->findChildren<TrafficItemView*>())
    {
        if(shift)
            agentView->setIndex(agentView->getIndex()-1);

        if(agentView->getID() == id)
        {
            ui->RegularLaneLayout->removeWidget(agentView);
            resize(width(), height() - agentView->height());
            delete agentView;
            shift = true;
        }
    }
}

void TrafficLanesView::removeRightmostLaneAgentView(unsigned int id)
{
    bool shift = false;

    for(auto agentView : ui->RightmostLaneWidget->findChildren<TrafficItemView*>())
    {
        if(shift)
            agentView->setIndex(agentView->getIndex()-1);

        if(agentView->getID() == id)
        {
            ui->RightmostLaneLayout->removeWidget(agentView);
            resize(width(), height() - agentView->height());
            delete agentView;
            shift = true;
        }
    }
}

void TrafficLanesView::setAgentSelectionItems(const QStringList &items)
{
    ui->RegularLaneAgent->clear();
    ui->RegularLaneAgent->addItems(items);

    ui->RightmostLaneAgent->clear();
    ui->RightmostLaneAgent->addItems(items);
}

void TrafficLanesView::resizeEvent(QResizeEvent *event)
{
    event->ignore();

    Q_EMIT resized();
}


void TrafficLanesView::setRegularLaneAgentProbability(unsigned int id, double value)
{
    for(auto agentView: ui->RegularLaneWidget->findChildren<TrafficItemView*>())
    {
        if(agentView->getID() == id)
            agentView->setProbability(value);
    }
}

void TrafficLanesView::setRightmostLaneAgentProbability(unsigned int id, double value)
{
    for(auto agentView: ui->RightmostLaneWidget->findChildren<TrafficItemView*>())
    {
        if(agentView->getID() == id)
            agentView->setProbability(value);
    }
}

void TrafficLanesView::clearTrafficLanesView()
{
    qDeleteAll(findChildren<TrafficItemView*>());
    resize(width(), initialHeight);

}
