#include "Views/SystemTabView.h"
#include "Views/SystemItemView.h"

#include "ui_SystemTabView.h"

#include <QMessageBox>
#include <QInputDialog>

SystemTabView::SystemTabView(SystemMapInterface * const systems,
                             const SystemComponentManagerInterface * const components,
                             bool const * const dynamicMode,
                             QWidget * const parent) :
    WidgetView(parent),
    systems(systems),
    components(components),
    dynamicMode(dynamicMode),
    ui(new Ui::SystemTabView)
{
    ui->setupUi(this);

    // set tabs closable and movable
    ui->systemTabs->setTabsClosable(true);
    ui->systemTabs->setMovable(true);

    connect(ui->systemTabs, &QTabWidget::tabCloseRequested, this, &SystemTabView::removeTab);
    connect(systems, &SystemMapInterface::modifiedTitle, this, &SystemTabView::changeTabTitle);
    connect(systems, &SystemMapInterface::added, this, &SystemTabView::addTab);
    connect(systems, &SystemMapInterface::cleared, this, &SystemTabView::clear);

}

SystemTabView::~SystemTabView()
{
    delete ui;
}

void SystemTabView::on_systemTabs_tabBarDoubleClicked(int index)
{
    QString newName = QInputDialog::getText(
                 this, tr ("Change Name"),
                 tr ("Insert New Name"),
                 QLineEdit::Normal);

    if(!newName.isEmpty())
    {
        ui->systemTabs->setTabText(index, newName);
        SystemItemView *view= qobject_cast<SystemItemView*>( ui->systemTabs->widget(index) );

        if(0!=view)
            view->getSystem()->setTitle(newName);
    }
}

void SystemTabView::addTab(SystemItemInterface * const system)
{
    SystemItemView * view = new SystemItemView(system, components, dynamicMode, this);
    ui->systemTabs->addTab(view, system->getTitle());
    ui->systemTabs->setCurrentWidget(view);
}

void SystemTabView::removeTab(int index)
{
    QMessageBox warning;
    SystemItemView *view = qobject_cast<SystemItemView*> (ui->systemTabs->widget(index));
    int ret;

    if(0!=view)
    {
        warning.setText("The whole System will be deleted!");
        warning.setInformativeText("Do you want to continue?");
        warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ret=warning.exec();

        if(ret==QMessageBox::Yes)
        {
            SystemItemInterface* system=view->getSystem();
            ui->systemTabs->removeTab(index);
            delete view;
            systems->remove(system->getID());
        }
    }
}

void SystemTabView::clear()
{
    qDeleteAll(findChildren<SystemItemView*>());
    ui->systemTabs->clear();
}


void SystemTabView::changeTabTitle(const SystemMapInterface::ID &id)
{
    int tabID;
    SystemItemView *view;

    for(tabID=0; tabID<ui->systemTabs->count();tabID++)
    {
       view = qobject_cast<SystemItemView*>(ui->systemTabs->widget(tabID));

       if(0!=view)
           if(view->getSystem()->getID() == id)
                break;
    }

    ui->systemTabs->setTabText(tabID, systems->getItem(id)->getTitle());
}
