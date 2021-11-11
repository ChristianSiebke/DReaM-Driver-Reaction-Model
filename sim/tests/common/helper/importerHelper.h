/********************************************************************************
 * Copyright (c) 2018-2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include <QDomDocument>
#include <QString>

namespace
{
    static QDomElement documentRootFromString(std::string content)
    {
        static QDomDocument document;
        document.setContent(QString::fromStdString(content));
        return document.documentElement();
    }
}
