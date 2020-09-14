#include "ScenarioBaseView.h"

ScenarioBaseView::ScenarioBaseView(QWidget *parent) :
    WidgetView(parent)
{
    scenarioView = new ScenarioView(this);

    connect(scenarioView, &ScenarioView::resized, this, &ScenarioBaseView::updateSize);

    adjustWidgets(parent);
}

ScenarioView *ScenarioBaseView::getScenarioView() const
{
    return scenarioView;
}

void ScenarioBaseView::updateSize()
{
    QRect frame(0, 0, 1, 1);

    for (auto child : findChildren<WidgetView *>())
        frame = frame.united(child->geometry());

    setGeometry(frame);
}

void ScenarioBaseView::adjustWidgets(QWidget *mainWindow)
{
    int x;

    // ajust the position of Scenario View:
    x = (mainWindow->width() - scenarioView->width()) / 2;
    scenarioView->move(x, 30);

    updateSize();
}
