/********************************************************************************
 * Copyright (c) 2017-2018 ITK Engineering GmbH
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QMap>
#include <list>
#include <string>
#include "common/log.h"

class ProcessManager : public QObject
{
    Q_OBJECT
public:
    static ProcessManager& getInstance();

    ProcessManager(const ProcessManager&) = delete;
    ProcessManager(ProcessManager&&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;
    ProcessManager& operator=(ProcessManager&&) = delete;
    virtual ~ProcessManager() { KillAll(); }

    bool StartProcess(const std::string& processPath, const std::vector<std::pair<std::string, std::string>>& arguments);
    void WaitAndClear();
    void KillAll();

signals:

public slots:

private:
    ProcessManager(QObject* parent = nullptr);
    void RemoveProcess(QProcess* process);

    int idealProcessCount;
    QMap<QProcess*, int> processMap;
};
