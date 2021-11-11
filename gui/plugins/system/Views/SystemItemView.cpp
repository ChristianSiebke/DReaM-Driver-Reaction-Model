/********************************************************************************
 * Copyright (c) 2017 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "Views/SystemItemView.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QModelIndex>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QVariant>
#include <QWheelEvent>

#include "Views/SystemBaseView.h"
#include "openPASS-System/SystemComponentManagerInterface.h"
#include "openPASS-System/SystemItemInterface.h"

QString const SystemItemView::DragDropMimeDataFormat =
    QStringLiteral("application/x-qabstractitemmodeldatalist");
QPoint const SystemItemView::MouseInvalidPosition = QPoint(-1, -1);

SystemItemView::SystemItemView(SystemItemInterface *const system,
                               SystemComponentManagerInterface const *const components,
                               bool const *const dynamicMode,
                               QWidget *const parent) :
    QScrollArea(parent), system(system), components(components), dynamicMode(dynamicMode)

{
    setAcceptDrops(true);
    setFrameShape(QFrame::NoFrame);
    setWidget(new SystemBaseView(system, this));
}

void SystemItemView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(DragDropMimeDataFormat))
    {
        event->acceptProposedAction();
    }
}

void SystemItemView::dropEvent(QDropEvent *event)
{
    QMessageBox message;

    // Info about restricted mode
    message.setText("You are working in dynamic mode. In this mode, you can only build systems which are "
                    "compatible with a profile-based agent configuration."
                    " In the current release, you are limited to one algorithm and multiple sensors.");

    message.setInformativeText("Please, reconsider your action.");
    message.setStandardButtons(QMessageBox::Ok);

    if (event->mimeData()->hasFormat(DragDropMimeDataFormat))
    {
        QStandardItemModel *const item = new QStandardItemModel();
        if (item->dropMimeData(event->mimeData(), Qt::CopyAction, 0, 0, QModelIndex()))
        {
            ComponentItemInterface *component = components->lookupItemByTitle(item->data(item->index(0, 0)).toString());

            if (*dynamicMode)
            {
                ComponentItemInterface::Type type = component->getType();

                if ((type == ComponentItemInterface::Type::Algorithm && !algoIncluded()) || type == ComponentItemInterface::Type::Sensor)
                    system->getComponents()->add(component, widget()->mapFromParent(event->pos()));
                else
                    message.exec();
            }
            else
                system->getComponents()->add(component, widget()->mapFromParent(event->pos()));
        }
        delete item;
        event->accept();
    }
}

void SystemItemView::mousePressEvent(QMouseEvent *event)
{
    mouseGlobalPosition = MouseInvalidPosition;
    if (event->button() == Qt::RightButton)
    {
        mouseGlobalPosition = event->globalPos();
    }
}

void SystemItemView::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseGlobalPosition != MouseInvalidPosition)
    {
        QPoint const mouseDelta = (event->globalPos() - mouseGlobalPosition) * 2;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - mouseDelta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - mouseDelta.y());
        mouseGlobalPosition = event->globalPos();
    }
}

void SystemItemView::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

bool SystemItemView::algoIncluded()
{
    for (auto item : *(system->getComponents()))
        if (item->getType() == ComponentItemInterface::Type::Algorithm)
            return true;

    return false;
}

SystemItemInterface *SystemItemView::getSystem() const
{
    return system;
}
