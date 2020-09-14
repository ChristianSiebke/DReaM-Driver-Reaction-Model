#ifndef ENVIRONMENTBASEVIEW_H
#define ENVIRONMENTBASEVIEW_H

#include "EnvironmentView.h"
#include "WidgetView.h"

class EnvironmentBaseView : public WidgetView
{
public:
    EnvironmentBaseView(QWidget *parent = 0);
    ~EnvironmentBaseView() = default;

public Q_SLOTS:
    void updateSize();
    void adjustWidgets(QWidget *mainWindow);

public:
    EnvironmentView *getEnvironmentView() const;

public:
    EnvironmentView *environmentView;
};

#endif // ENVIRONMENTBASEVIEW_H
