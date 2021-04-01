/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef RESULTDIRITEMMODEL_H
#define RESULTDIRITEMMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>

/*!
 * \brief The Result Directory itemModel
 * This class is responsible to to create and manage an itemModel for the result directory
 */
class ResultDirItemModel : public QStandardItemModel
{

public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     parent   parent object
    //-----------------------------------------------------------------------------
    ResultDirItemModel(QObject *parent = nullptr);

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    virtual ~ResultDirItemModel();

    //-----------------------------------------------------------------------------
    //! Set the root result folder directory.
    //! @return     resultDir   directory of the result folder
    //-----------------------------------------------------------------------------
    void setRootResultDir(QString resultDir);

private:
    QStandardItem *rootItem; //!< root item
    QIcon dirIcon;           //!< icon of a directory
    QIcon fileIcon;          //!< icon of a file

    //-----------------------------------------------------------------------------
    //! Create a directory item, to build up a tree recursivly
    //! @param[in]      dirPath     path of the directory
    //! @param[in]      parentItem  parent standard item
    //! @param[in]      dirDepth    depth of current directory
    //-----------------------------------------------------------------------------
    void createDirectoryItem(QString dirPath, QStandardItem *parentItem, int dirDepth);
};

#endif // RESULTDIRITEMMODEL_H
