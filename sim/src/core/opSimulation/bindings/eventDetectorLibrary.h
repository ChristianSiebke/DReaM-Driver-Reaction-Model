/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  EventDetectorLibrary.h
//! @brief This file contains the internal representation of the library of a
//!        event detector.
//-----------------------------------------------------------------------------

#pragma once

#include <QLibrary>
#include "bindings/eventDetectorBinding.h"
#include "include/eventDetectorInterface.h"
#include "include/callbackInterface.h"

namespace core
{

class EventDetectorLibrary
{
public:
    typedef const std::string &(*EventDetectorInterface_GetVersion)();
    typedef EventDetectorInterface *(*EventDetectorInterface_CreateCollisionDetectorInstanceType)(WorldInterface *world,
                                                                                 EventNetworkInterface* eventNetwork,
                                                                                 const CallbackInterface *callbacks,
                                                                                 StochasticsInterface *stochastics);
    typedef EventDetectorInterface *(*EventDetectorInterface_CreateConditionalDetectorInstanceType)(WorldInterface *world,
                                                                                 const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation,
                                                                                 EventNetworkInterface* eventNetwork,
                                                                                 const CallbackInterface *callbacks,
                                                                                 StochasticsInterface *stochastics);
    typedef void (*EventDetectorInterface_DestroyInstanceType)(EventDetectorInterface *implementation);

    EventDetectorLibrary(const std::string &libraryPath,
                         CallbackInterface *callbacks) :
        libraryPath(libraryPath),
        callbacks(callbacks)
    {}
    EventDetectorLibrary(const EventDetectorLibrary&) = delete;
    EventDetectorLibrary(EventDetectorLibrary&&) = delete;
    EventDetectorLibrary& operator=(const EventDetectorLibrary&) = delete;
    EventDetectorLibrary& operator=(EventDetectorLibrary&&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor, deletes the stored library (unloads it if necessary), if the list
    //! of stored event detectors is empty.
    //-----------------------------------------------------------------------------
    virtual ~EventDetectorLibrary();

    //-----------------------------------------------------------------------------
    //! Creates a QLibrary based on the path from the constructor and stores function
    //! pointer for getting the library version, creating and destroying instances
    //! and setting the spawn item (see typedefs for corresponding signatures).
    //!
    //! @return                 Null pointer
    //-----------------------------------------------------------------------------
    bool Init();

    //-----------------------------------------------------------------------------
    //! Find event detector in the sored list of event detectors, then call the "destroy
    //! instance" function pointer with its implementation and remove it from the list
    //! of stored event detectors.
    //!
    //! @param[in]  eventDetector          event detector to release
    //! @return                         Flag if the release was successful
    //-----------------------------------------------------------------------------
    bool ReleaseEventDetector(EventDetector *eventDetector);

    //-----------------------------------------------------------------------------
    //! Make sure that the library exists and is loaded, then call the "create instance"
    //! function pointer using the parameters from the event detector instance to get
    //! a event detector interface, which is then used to instantiate a event detector
    //! which is also stored in the list of event detectors.
    ///
    /// \param eventDetectorType
    /// \param parameters
    /// \param eventNetwork
    /// \param world
    /// \param stochastics
    /// \return instance of event detector
    ///
    EventDetector *CreateCollisionDetector(EventNetworkInterface *eventNetwork,
                                           WorldInterface *world,
                                           StochasticsInterface *stochastics);

    EventDetector *CreateConditionalDetector(const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation,
                                             EventNetworkInterface *eventNetwork,
                                             WorldInterface *world,
                                             StochasticsInterface *stochastics);

private:
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateCollisionDetectorInstanceId = "OpenPASS_CreateCollisionDetectorInstance";
    const std::string DllCreateConditionalDetectorInstanceId = "OpenPASS_CreateConditionalDetectorInstance";
    const std::string DllDestroyInstanceId = "OpenPASS_DestroyInstance";

    std::string libraryPath;
    std::vector<EventDetector*> eventDetectors;
    QLibrary *library = nullptr;
    CallbackInterface *callbacks;
    EventDetectorInterface_GetVersion getVersionFunc{nullptr};
    EventDetectorInterface_CreateCollisionDetectorInstanceType createCollisionDetectorInstanceFunc{nullptr};
    EventDetectorInterface_CreateConditionalDetectorInstanceType createConditionalDetectorInstanceFunc{nullptr};
    EventDetectorInterface_DestroyInstanceType destroyInstanceFunc{nullptr};
};

} // namespace core
