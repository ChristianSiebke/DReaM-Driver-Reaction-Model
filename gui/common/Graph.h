/********************************************************************************
 * Copyright (c) 2017-2020 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QGraphicsRectItem>

class Graph : public QGraphicsRectItem
{
public:
    Graph(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = nullptr);
};
