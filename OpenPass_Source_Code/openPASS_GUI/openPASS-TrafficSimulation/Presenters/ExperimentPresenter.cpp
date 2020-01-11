/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#include "Presenters/ExperimentPresenter.h"

#include "Views/DelegateFileBrowseView.h"

#include "DelegateComboBoxView.h"

QStringList ExperimentPresenter::loggingGroupsList =
{
    "Trace",
    "Visualization",
    "RoadPosition",
    "Sensor",
    "Driver",
};

QStringList ExperimentPresenter::libraryTypesList =
{
    "EventDetectorLibrary",
    "ManipulatorLibrary",
    "SpawnPointLibrary",
    "StochasticsLibrary",
    "WorldLibrary",
    "ObservationLibrary",
};

ExperimentPresenter::ExperimentPresenter(ExperimentInterface * const experiment,
                                         ExperimentView * const experimentView,
                                         ProjectInterface * const project,
                                         QUndoStack * const undoStack,
                                         QObject *parent )
    : QObject(parent)
    , experiment(experiment)
    , experimentView(experimentView)
    , project(project)
    , undoStack(undoStack)
    , librariesPresenter( new LibrariesPresenter(experiment->getLibraries(), project,this))
    , loggingGroupsPresenter( new LoggingGroupsPresenter(experiment->getLoggingGroups(), this) )
{
    // Connect the signals
    connect(experimentView, &ExperimentView::Invocations_valueChanged, this, &ExperimentPresenter::setNumberInvocations);
    connect(experimentView, &ExperimentView::Seed_valueChanged, this, &ExperimentPresenter::setSeed);
    connect(experimentView, &ExperimentView::startSimulation, this, &ExperimentPresenter::startSimulation);

    // setup table of libraries and list of logging groups
    setupLibraries();
    setupLoggingGroups();

    updateView();
}

void ExperimentPresenter::setupLoggingGroups()
{
    QListView * const loggingGroupsView = experimentView->getLoggingGroupsView();
    loggingGroupsView->setModel(loggingGroupsPresenter);

    loggingGroupsView->setItemDelegate(new DelegateComboBoxView(&loggingGroupsList, "logging group", undoStack,  this) );

    connect(experimentView, &ExperimentView::addLoggingGroup, this, &ExperimentPresenter::addLoggingGroup);
    connect(experimentView, &ExperimentView::removeLoggingGroup, this, &ExperimentPresenter::removeLoggingGroup);
}

void ExperimentPresenter::addLoggingGroup()
{
    for(auto item : loggingGroupsList)
    {
        if(!experiment->getLoggingGroups()->contains(item))
        {
            undoStack->push(new AddLoggingGroup(loggingGroupsPresenter, item));
            break;
        }
    }
}

void ExperimentPresenter::removeLoggingGroup(const int &row)
{

    if(row >=0 && loggingGroupsPresenter->rowCount() != 0)
        undoStack->push(new RemoveLoggingGroup(loggingGroupsPresenter, row));
}

void ExperimentPresenter::setupLibraries()
{
    QTableView * const librariesView = experimentView->getLibrariesView();
    librariesView->setModel(librariesPresenter);

    librariesView->setItemDelegateForColumn(0, new DelegateComboBoxView(&libraryTypesList, "library type", undoStack, this) );
    librariesView->setItemDelegateForColumn(1, new DelegateFileBrowseView(project, "library", undoStack, this));

    connect(experimentView, &ExperimentView::addLibrary, this, &ExperimentPresenter::addLibrary);
    connect(experimentView, &ExperimentView::removeLibrary, this, &ExperimentPresenter::removeLibrary);
}

void ExperimentPresenter::addLibrary()
{
    for(auto libType : libraryTypesList)
    {
        if(!experiment->getLibraries()->keys().contains(libType))
        {
            undoStack->push(new AddLibrary(librariesPresenter, libType));
            break;
        }
    }
}

void ExperimentPresenter::removeLibrary(const int &row)
{
    if(librariesPresenter->rowCount() !=0)
        undoStack->push(new RemoveLibrary(librariesPresenter, row));
}

void ExperimentPresenter::setNumberInvocations(ExperimentInterface::NumberInvocations _numberInvocations)
{
    if(experiment->getNumberInvocations() != _numberInvocations)
        undoStack->push(new EditNumberInvocations(experiment, experimentView, _numberInvocations));
}

void ExperimentPresenter::setSeed(ExperimentInterface::Seed _seed)
{
    if(experiment->getSeed() != _seed)
        undoStack->push(new EditRandomSeed(experiment, experimentView, _seed));
}

void ExperimentPresenter::updateView()
{

    experimentView->setInvocationsValue(experiment->getNumberInvocations());
    experimentView->setSeedValue(experiment->getSeed());

    loggingGroupsPresenter->update();
    librariesPresenter->update();
}


AddLibrary::AddLibrary(LibrariesPresenter * const table,
                       QString const &libType,
                       QUndoCommand *parent)
    : QUndoCommand (parent)
    , table(table)
    , libType(libType)
{
    setText("added entry to libraries table");
}

void AddLibrary::redo()
{
    table->addRow(table->rowCount(), libType);
}

void AddLibrary::undo()
{
    table->removeRow(table->rowCount()-1);
}

RemoveLibrary::RemoveLibrary(LibrariesPresenter * const table,
                             int const &row,
                             QUndoCommand *parent)
    : QUndoCommand (parent)
    , table(table)
    , row(row)
    , libType(table->data(table->index(row,0)).toString())
    , lib(table->data(table->index(row,1)).toString())

{
    setText("added entry to libraries table");
}

void RemoveLibrary::redo()
{
    table->removeRow(row);
}

void RemoveLibrary::undo()
{
    table->addRow(row, libType);
    table->setData(table->index(row, 1), lib);
}

AddLoggingGroup::AddLoggingGroup(LoggingGroupsPresenter * const list,
                                 QString const & name,
                                 QUndoCommand *parent)
    : QUndoCommand(parent)
    , list(list)
    , name(name)
{
    setText("added entry to logging groups list");
}

void AddLoggingGroup::redo()
{
    list->insertRow(list->rowCount(), name);
}

void AddLoggingGroup::undo()
{
    list->removeRow(list->rowCount()-1);
}

RemoveLoggingGroup::RemoveLoggingGroup(LoggingGroupsPresenter * const list,
                                       int const &row,
                                       QUndoCommand *parent)
    : QUndoCommand(parent)
    , list(list)
    , row(row)
    , name(list->data(list->index(row)).toString())
{
    setText("removed entry from logging groups list");
}

void RemoveLoggingGroup::redo()
{
    list->removeRow(row);
}

void RemoveLoggingGroup::undo()
{
    list->insertRow(row, name);
}


EditNumberInvocations::EditNumberInvocations(ExperimentInterface * const experiment,
                                             ExperimentView * const experimentView,
                                             const ExperimentInterface::NumberInvocations &newValue,
                                             QUndoCommand *parent)
    : QUndoCommand (parent)
    , experiment(experiment)
    , experimentView(experimentView)
    , newValue(newValue)
    , oldValue(experiment->getNumberInvocations())
{
    setText("edited number of invocations");
}

void EditNumberInvocations::redo()
{
    experiment->setNumberInvocations(newValue);
    experimentView->setInvocationsValue(newValue);
}

void EditNumberInvocations::undo()
{
    experiment->setNumberInvocations(oldValue);
    experimentView->setInvocationsValue(oldValue);
}

EditRandomSeed::EditRandomSeed(ExperimentInterface * const experiment,
                               ExperimentView * const experimentView,
                               const ExperimentInterface::Seed &newValue,
                               QUndoCommand *parent)
    : QUndoCommand (parent)
    , experiment(experiment)
    , experimentView(experimentView)
    , newValue(newValue)
    , oldValue(experiment->getSeed())
{
    setText("edited random seed");
}

void EditRandomSeed::redo()
{
    experiment->setSeed(newValue);
    experimentView->setSeedValue(newValue);
}

void EditRandomSeed::undo()
{
    experiment->setSeed(oldValue);
    experimentView->setSeedValue(oldValue);
}
