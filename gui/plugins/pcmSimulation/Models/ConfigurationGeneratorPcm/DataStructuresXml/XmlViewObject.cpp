/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XmlViewObject.h"

XmlViewObject::XmlViewObject(const PCM_ViewObject *viewObject)
{
    this->viewObject = viewObject;
}

bool XmlViewObject::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    if (viewObject != nullptr)
    {
        xmlWriter->writeStartElement(QString::fromStdString(PCM_Helper::ConvertObjectTypeToDBString(
                                                                ObjectType::VIEWOBJECT)));
        const std::map<int, PCM_Line *> *lineMap = viewObject->GetLineMap();
        for (std::pair< int, PCM_Line *> linePair : *lineMap)
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
