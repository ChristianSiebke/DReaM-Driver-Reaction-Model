/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ManipulatorLibrary.h
//! @brief This file contains the internal representation of the library of a
//!        manipulator.
//-----------------------------------------------------------------------------

#pragma once

#include <QLibrary>
#include "bindings/manipulatorBinding.h"
#include "include/manipulatorInterface.h"
#include "include/callbackInterface.h"

class PublisherInterface;

namespace core
{

class ManipulatorLibrary
{
public:
    typedef const std::string &(*ManipulatorInterface_GetVersion)();
    typedef ManipulatorInterface *(*ManipulatorInterface_CreateInstanceType)(WorldInterface *world,
                                                                             openScenario::ManipulatorInformation manipulatorInformation,
                                                                             EventNetworkInterface* eventNetwork,
                                                                             const CallbackInterface *callbacks);
    typedef ManipulatorInterface *(*ManipulatorInterface_CreateDefaultInstanceType)(WorldInterface *world,
                                                                             std::string manipulatorType,
                                                                             EventNetworkInterface* eventNetwork,
                                                                             const CallbackInterface *callbacks,
                                                                             PublisherInterface* publisher);
    typedef void (*ManipulatorInterface_DestroyInstanceType)(ManipulatorInterface *implementation);

    ManipulatorLibrary(const std::string &libraryPath,
                       CallbackInterface *callbacks) :
        libraryPath(libraryPath),
        callbacks(callbacks)
    {}
    ManipulatorLibrary(const ManipulatorLibrary&) = delete;
    ManipulatorLibrary(ManipulatorLibrary&&) = delete;
    ManipulatorLibrary& operator=(const ManipulatorLibrary&) = delete;
    ManipulatorLibrary& operator=(ManipulatorLibrary&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes the stored library (unloads it if necessary), if the list
    //! of stored spawn points is empty.
    //-----------------------------------------------------------------------------
    virtual ~ManipulatorLibrary();

    //-----------------------------------------------------------------------------
    //! Creates a QLibrary based on the path from the constructor and stores function
    //! pointer for getting the library version, creating and destroying instances
    //! and setting the spawn item (see typedefs for corresponding signatures).
    //!
    //! @return                 Null pointer
    //-----------------------------------------------------------------------------
    bool Init();

    //-----------------------------------------------------------------------------
    //! Find spawn point in the sored list of spawn points, then call the "destroy
    //! instance" function pointer with its implementation and remove it from the list
    //! of stored spawn points.
    //!
    //! @param[in]  spawnPoint          Spawn point to release
    //! @return                         Flag if the release was successful
    //-----------------------------------------------------------------------------
    bool ReleaseManipulator(Manipulator *manipulator);

    //-----------------------------------------------------------------------------
    //! Make sure that the library exists and is loaded, then call the "create instance"
    //! function pointer using the parameters from either the action interface
    //! or the ManipulatorParameters to get a manipulator
    //!
    //! @param[in]  action              Action interface containing manipulator
    //!                                 parameters
    //! @param[in]  manipulatorType     The type of the manipulator to be instantiated;
    //!                                 this is only necessary if no action is passed
    //! @param[in]  parameters          The parameters for the manipulator; this is
    //!                                 only necessary if no action is passed
    //! @param[in]  eventNetwork        The event network within which the manipulator inserts events
    //! @param[in]  world               World instance
    //! @return                         Manipulator created
    //-----------------------------------------------------------------------------
    Manipulator *CreateManipulator(const openScenario::ManipulatorInformation manipulatorInformation,
                                   EventNetworkInterface* eventNetwork,
                                   WorldInterface* world);

    Manipulator *CreateManipulator(const std::string& manipulatorType,
                                   EventNetworkInterface* eventNetwork,
                                   WorldInterface* world,
                                   PublisherInterface* publisher);

private:
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateInstanceId = "OpenPASS_CreateInstance";
    const std::string DllCreateDefaultInstanceId = "OpenPASS_CreateDefaultInstance";
    const std::string DllDestroyInstanceId = "OpenPASS_DestroyInstance";

    std::string libraryPath;
    std::vector<Manipulator*> manipulators;
    QLibrary *library = nullptr;
    CallbackInterface *callbacks;
    ManipulatorInterface_GetVersion getVersionFunc;
    ManipulatorInterface_CreateInstanceType createInstanceFunc;
    ManipulatorInterface_CreateDefaultInstanceType createDefaultInstanceFunc;
    ManipulatorInterface_DestroyInstanceType destroyInstanceFunc;
};

} // namespace core


