/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ScenarioAgentsPresenter.h"

#include <QBrush>

ScenarioAgentsPresenter::ScenarioAgentsPresenter(ScenarioInterface::Entities *const entities,
                                                 const QMap<QString, QList<QString>> &scenarioAgentsList,
                                                 QObject *parent) :
    QAbstractTableModel(parent), entities(entities), scenarioAgentsList(scenarioAgentsList)
{
    _columnHeaders << "Agent"
                   << "Profile Catalogue"
                   << "Agent Profile";
}

QVariant ScenarioAgentsPresenter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
        return _columnHeaders.value(section);

    if ((role == Qt::DisplayRole) && (orientation == Qt::Vertical))
        return section + 1;

    return QVariant();
}

int ScenarioAgentsPresenter::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return entities->count();
}

int ScenarioAgentsPresenter::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _columnHeaders.count();
}

QVariant ScenarioAgentsPresenter::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();
    QString name = RowToName.value(row);

    if (row >= entities->count())
        return QVariant::Invalid;

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignCenter);

    if (role == Qt::TextColorRole)
    {
        QBrush color(Qt::red);

        if (col == 1)
        {
            QString catalogue = entities->value(name).first;

            if (!scenarioAgentsList.contains(catalogue))
                return color;
        }
        else if (col == 2)
        {
            QString agentProfile = entities->value(name).second;
            QString catalogue = entities->value(name).first;

            if (!scenarioAgentsList.contains(catalogue))
                return color;
            else if (!scenarioAgentsList.value(catalogue).contains(agentProfile))
                return color;
        }

        return QBrush(Qt::black);
    }

    if (role == Qt::DisplayRole)
    {
        if (col == 0)
            return name;
        else if (col == 1)
        {
            QString catalogue = entities->value(name).first;

            if (!scenarioAgentsList.contains(catalogue))
            {
                return QString("%1 (inaccessible!)").arg(catalogue);
            }
            else
                return catalogue;
        }
        else if (col == 2)
        {
            QString agentProfile = entities->value(name).second;
            QString catalogue = entities->value(name).first;

            if (!scenarioAgentsList.contains(catalogue) || !scenarioAgentsList.value(catalogue).contains(agentProfile))
            {
                return QString("%1 (not existing!)").arg(agentProfile);
            }
            else
            {
                return agentProfile;
            }
        }
    }

    return QVariant();
}

void ScenarioAgentsPresenter::update()
{
    int rows = 0;

    beginResetModel();
    endResetModel();
    RowToName.clear();

    for (auto name : entities->keys())
    {
        RowToName.insert(rows, name);
        rows++;
    }
}
