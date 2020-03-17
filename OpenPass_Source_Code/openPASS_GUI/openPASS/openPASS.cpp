/******************************************************************************
* Copyright (c) 2017, 2019 Volkswagen Group of America.
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include <QApplication>
#include <QDir>
#include <QTimer>

#include "openPASS/RandomInterface.h"
#include "Models/ServiceManagerModel.h"
#include "Models/PluginManagerModel.h"

int main(int argc, char *argv[])
{
    // Initialize random interface
    RandomInterface::initialize();

    // Initialize application
    QApplication application(argc, argv);
    application.setApplicationName(QStringLiteral(APPLICATION_NAME));
    application.setApplicationVersion(QStringLiteral(APPLICATION_VERSION));
    application.addLibraryPath(application.applicationDirPath() + SUBDIR_LIB_GUI);
    application.addLibraryPath(application.applicationDirPath() + SUBDIR_LIB_GUI);

    // Initialize models
    ServiceManagerModel services;
    PluginManagerModel plugins(&services);

    // Schedule application
    QTimer::singleShot(0, [&plugins](){
        // Load plugins
        plugins.loadDirectory(QDir(QApplication::applicationDirPath()  + SUBDIR_LIB_GUI));

        // Emit signal 'started'
        Q_EMIT plugins.started();
    });

    // Execute application
    return application.exec();
}
