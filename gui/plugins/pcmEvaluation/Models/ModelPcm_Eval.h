/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef MODELPCM_EVAL_H
#define MODELPCM_EVAL_H

#include <QDir>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMap>
#include <QObject>

#include "ResultDirItemModel.h"
#include "TrajectoryTableModel.h"
#include "pcm_data.h"
#include "pcm_definitions.h"
#include "pcm_line.h"
#include "sceneryImporterPCM.h"

class ModelPcm_Eval : public QObject
{
    Q_OBJECT

public:
    explicit ModelPcm_Eval(QObject *parent = nullptr);
    virtual ~ModelPcm_Eval();

    QAbstractItemModel *GetItemModelPcm() const;
    void SetSelectionModelPcm(QItemSelectionModel *selectionModel);

public:
Q_SIGNALS:
    void TableModelCreated(QAbstractTableModel *newModel, const QString &tableName) const;
    void TrajectoryCreated(QVector<QPointF> *trajectoryData);
    void MarksCreated(QVector<QPointF> *marksData, MarkType markType);
    void ObjectCreated(QVector<QPointF> *objectData, ObjectType objectType);

    void Clear();

public Q_SLOTS:
    bool LoadFileData(const QString &fileName, unsigned indexAgent);
    bool LoadSceneryData(const QString &sceneryFile);
    void LoadPathData(const QString &pathName);

private Q_SLOTS:
    void OnSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QVector<QPointF> ConvertPcmLine(const PCM_Line *pcmLine, QVector<QPointF> &line);
    QMap<QString, TrajectoryTableModel *> tableMap;

    ResultDirItemModel *treeModelPcm = nullptr;
    QItemSelectionModel *selectionModelPcm = nullptr;
    QModelIndexList selectedIndexList;

    //    QStringList resultList;
    QString rootPath = "";
    const unsigned indexAgentMax = 100;
};

#endif // MODELPCM_EVAL_H
