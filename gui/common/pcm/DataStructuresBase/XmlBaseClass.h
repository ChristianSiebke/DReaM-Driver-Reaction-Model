/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef XMLBASECLASS_H
#define XMLBASECLASS_H

#include <QXmlStreamWriter>

/*!
 * \brief The xml base class.
 * This class is responsible to implement all basic functionality to write an xml.
 */
class XmlBaseClass
{
public:
    //-----------------------------------------------------------------------------
    //! Default Constructor
    //-----------------------------------------------------------------------------
    XmlBaseClass() = default;

    //-----------------------------------------------------------------------------
    //! Constructor
    //! @param[in] id id of the xml entry
    //-----------------------------------------------------------------------------
    XmlBaseClass(int id) :
        id(id)
    {
    }

    // removing operators
    XmlBaseClass(const XmlBaseClass &) = default;
    XmlBaseClass(XmlBaseClass &&) = default;
    XmlBaseClass &operator=(const XmlBaseClass &) = delete;
    XmlBaseClass &operator=(XmlBaseClass &&) = delete;

    //-----------------------------------------------------------------------------
    //! Default Destructor
    //-----------------------------------------------------------------------------
    virtual ~XmlBaseClass() = default;

    //-----------------------------------------------------------------------------
    //! @brief Write data in an xml file.
    //! @param[in] xmlWriter    specific writer which shall be used to write its
    //!                         data to an xml file.
    //! @return     true for a successful write
    //-----------------------------------------------------------------------------
    virtual bool WriteToXml(QXmlStreamWriter *xmlWriter) = 0;

    int id = -1; //!< the id of the xml-entry
};

#endif // XMLBASECLASS_H
