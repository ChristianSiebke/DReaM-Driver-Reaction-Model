/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "XmlComponentObservation.h"

XmlComponentObservation::XmlComponentObservation( int id, int observationRef ):
    XmlBaseClass(id), observationRef(observationRef)
{}

bool XmlComponentObservation::WriteToXml(QXmlStreamWriter *xmlWriter)
{
    if (xmlWriter == nullptr)
    {
        return false;
    }

    xmlWriter->writeStartElement("ComponentObservation");

    xmlWriter->writeAttribute("id", QString::number(id));
    xmlWriter->writeAttribute("observationRef", QString::number(observationRef));

    xmlWriter->writeEndElement();

    return true;
}
