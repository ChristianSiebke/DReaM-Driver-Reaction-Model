/*********************************************************************
* Copyright (c) 2017, 2018, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "Graph.h"

Graph::Graph(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) : QGraphicsRectItem(0.0, 0.0, w, h, parent)
{
    this->setPos(x, y);
}
