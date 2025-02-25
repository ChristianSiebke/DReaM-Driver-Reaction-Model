/******************************************************************************
 * Copyright (c) 2022 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "GlobalObserver.h"

#include "GlobalObserver_implementation.h"

const std::string Version = "0.0.1";
static const CallbackInterface *Callbacks = nullptr;

extern "C" GLOBALOBSERVER_SHARED_EXPORT const std::string &OpenPASS_GetVersion() {
    return Version;
}

extern "C" GLOBALOBSERVER_SHARED_EXPORT ModelInterface *
OpenPASS_CreateInstance(std::string componentName, bool isInit, int priority, int offsetTime, int responseTime, int cycleTime,
                        StochasticsInterface *stochastics, WorldInterface *world, const ParameterInterface *parameters,
                        PublisherInterface *const publisher, AgentInterface *agent, const CallbackInterface *callbacks) {
    Callbacks = callbacks;

    try {
        return (ModelInterface *)(new (std::nothrow)
                                      GlobalObserver_Implementation(componentName, isInit, priority, offsetTime, responseTime, cycleTime,
                                                                    stochastics, world, parameters, publisher, callbacks, agent));
    }

    catch (const std::runtime_error &ex) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return nullptr;
    }
    catch (...) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}

extern "C" GLOBALOBSERVER_SHARED_EXPORT void OpenPASS_DestroyInstance(ModelInterface *implementation) {
    delete (GlobalObserver_Implementation *)implementation;
}

extern "C" GLOBALOBSERVER_SHARED_EXPORT bool OpenPASS_UpdateInput(ModelInterface *implementation, int localLinkId,
                                                                  const std::shared_ptr<SignalInterface const> &data, int time) {
    try {
        implementation->UpdateInput(localLinkId, data, time);
    }
    catch (const std::runtime_error &ex) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" GLOBALOBSERVER_SHARED_EXPORT bool OpenPASS_UpdateOutput(ModelInterface *implementation, int localLinkId,
                                                                   std::shared_ptr<SignalInterface const> &data, int time) {
    try {
        implementation->UpdateOutput(localLinkId, data, time);
    }
    catch (const std::runtime_error &ex) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" GLOBALOBSERVER_SHARED_EXPORT bool OpenPASS_Trigger(ModelInterface *implementation, int time) {
    try {
        implementation->Trigger(time);
    }
    catch (const std::runtime_error &ex) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...) {
        if (Callbacks != nullptr) {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}
