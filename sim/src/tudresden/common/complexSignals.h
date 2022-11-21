/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#pragma once

#include "include/modelInterface.h"

template <class T> class structSignal : public SignalInterface {
public:
    const std::string SignalName = "structSignal";

    structSignal(const T inValue) : value(inValue) {
    }
    structSignal(const structSignal &) = default;
    structSignal(structSignal &&) = default;
    structSignal &operator=(const structSignal &) = default;
    structSignal &operator=(structSignal &&) = default;
    virtual ~structSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------

    virtual operator std::string() const {
        return SignalName;
    }

    const T value; //!< signal content
};

template <typename Container, typename T = typename Container::value_type> class ContainerSignal : public SignalInterface {
public:
    const std::string SignalName = "ContainerSignal";
    ContainerSignal(const Container inValue) : value(inValue) {
    }
    ContainerSignal(const ContainerSignal &) = default;
    ContainerSignal(ContainerSignal &&) = default;
    ContainerSignal &operator=(const ContainerSignal &) = default;
    ContainerSignal &operator=(ContainerSignal &&) = default;
    virtual ~ContainerSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------

    virtual operator std::string() const {
        return SignalName;
    }

    const Container value; //!< signal content
};

//------------------------------------------------------
template <class T1, class T2> class pairSignal : public SignalInterface {
public:
    const std::string SignalName = "pairSignal";

    pairSignal(std::pair<T1, T2> inValue) : value{inValue} {
    }
    pairSignal(const pairSignal &) = default;
    pairSignal(pairSignal &&) = default;
    pairSignal &operator=(const pairSignal &) = default;
    pairSignal &operator=(pairSignal &&) = default;
    virtual ~pairSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------

    virtual operator std::string() const {
        return SignalName;
    }

    std::pair<T1, T2> value; //!< signal content
};
