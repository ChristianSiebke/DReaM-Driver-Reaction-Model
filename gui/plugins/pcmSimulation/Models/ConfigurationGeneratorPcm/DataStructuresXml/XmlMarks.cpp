/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XmlMarks.h"

XmlMarks::XmlMarks(const PCM_Marks *marks)
{
    this->marks = marks;
}

bool XmlMarks::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    if (marks != nullptr)
    {
        xmlWriter->writeStartElement(QString::fromStdString(PCM_Helper::ConvertMarkTypeToDBString(
                                                                marks->GetMarkType())));
        const std::map<int, PCM_Line *> *lineMap = marks->GetLineMap();
        for (std::pair<int, PCM_Line *> linePair : *lineMap)
        {
            PCM_Line *line = linePair.second;
            XmlLine xmlLine(line);
            if (!xmlLine.WriteToXml(xmlWriter))
            {
                return false;
            }
        }
        xmlWriter->writeEndElement();
    }

    return true;
}
