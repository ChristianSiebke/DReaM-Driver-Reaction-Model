/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

#include "Views/SystemComponentManagerView.h"
#include "ui_SystemComponentManagerView.h"

#include "openPASS-System/SystemComponentManagerInterface.h"

SystemComponentManagerView::SystemComponentManagerView(SystemComponentManagerInterface * const components,
                                                       const bool * const dynamicMode,
                                                       QWidget * const parent)
    : QWidget(parent)
    , _components(components)
    , dynamicMode(dynamicMode)
    , ui(new Ui::SystemComponentManagerView)
{
    // Setup UI
    ui->setupUi(this);

    // Update the tree view
    updateManagerView();
}

SystemComponentManagerView::~SystemComponentManagerView()
{
    // Destroy UI
    delete ui;
}

void SystemComponentManagerView::show()
{
    // Maximize widget
    ui->button->setChecked(true);
    ui->tree->setVisible(true);
}

void SystemComponentManagerView::hide()
{
    // Minimize widget
    ui->button->setChecked(false);
    ui->tree->setVisible(false);

    // Update the components tree
    updateManagerView();
}

void SystemComponentManagerView::updateManagerView()
{
    // Clear the tree of components
    for (unsigned char index = 0; index < ui->tree->topLevelItemCount(); ++index)
    {
        ui->tree->topLevelItem(index)->takeChildren();
    }

    // Load again the components
    if (_components->loadFromDirectory(QDir(QCoreApplication::applicationDirPath() + SUBDIR_LIB_COMPONENT)))
    {
            for (unsigned char index = 0; index < ui->tree->topLevelItemCount(); ++index)
            {
                ComponentItemInterface::Type type = static_cast<ComponentItemInterface::Type>(index);

                for (ComponentItemInterface::Title const & title : _components->listTitlesByType(type))
                {

                    QTreeWidgetItem *item = new QTreeWidgetItem(ui->tree->topLevelItem(index), {title}, 0);

                    // conditions for dynamic mode. Item is removed if component is not compatible with dynamic mode
                    if ( isForbidden(type, title) )
                    {
                        ui->tree->topLevelItem(index)->removeChild(item);
                        delete item;
                    }

                }
            }
        ui->tree->expandAll();
    }
}

void SystemComponentManagerView::on_button_clicked()
{
    // Toggle widget
    ui->button->setChecked(ui->button->isChecked());
    ui->tree->setVisible(!ui->tree->isVisible());

    if (ui->button->isChecked())
    {
        updateManagerView();
    }
}

int SystemComponentManagerView::numberOfInputs(const ComponentItemInterface::Title &title)
{
    ComponentItemInterface const * const component = _components->lookupItemByName(title);

    return component->getInputs()->count();
}

int SystemComponentManagerView::numberOfOutputs(const ComponentItemInterface::Title &title)
{
    ComponentItemInterface const * const component = _components->lookupItemByName(title);

    return component->getOutputs()->count();
}

int SystemComponentManagerView::numberOfParameters(const ComponentItemInterface::Title &title)
{
    ComponentItemInterface const * const component = _components->lookupItemByName(title);

    return component->getParameters()->count();
}

bool SystemComponentManagerView::isForbidden(const ComponentItemInterface::Type &type,
                                                      const ComponentItemInterface::Title &title)
{
    switch(type)
    {
        case ComponentItemInterface::Type::Action:
            return *dynamicMode;

        case ComponentItemInterface::Type::Sensor:
            return *dynamicMode && (numberOfInputs(title) != 0 || numberOfOutputs(title) !=1 || numberOfParameters(title) <6 );

        case ComponentItemInterface::Type::Algorithm:
            return *dynamicMode && ( numberOfOutputs(title) !=0 );

        default:
            return false;
    }
}
