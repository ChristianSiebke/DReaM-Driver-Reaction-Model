/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dataBuffer.h
//! @brief This file provides the interface to a data buffer implementation
//-----------------------------------------------------------------------------

#pragma once

#include "include/dataBufferInterface.h"
#include "bindings/dataBufferBinding.h"

namespace core {

class DataBuffer : public DataBufferInterface
{
public:
    DataBuffer(DataBufferBinding *dataBufferBinding) :
        dataBufferBinding(dataBufferBinding)
    {
    }

    DataBuffer(const DataBuffer &) = delete;
    DataBuffer(DataBuffer &&) = delete;
    DataBuffer &operator=(const DataBuffer &) = delete;
    DataBuffer &operator=(DataBuffer &&) = delete;
    ~DataBuffer() override = default;

    bool Instantiate() override;
    bool isInstantiated() const override;

    void PutCyclic(const openpass::type::EntityId entityId, const Key &key, const Value &value) override;
    void PutAcyclic(const openpass::type::EntityId entityId, const Key &key, const Acyclic &acyclic) override;
    void PutStatic(const Key &key, const Value &value, bool persist) override;

    void ClearRun() override;
    void ClearTimeStep() override;

    std::unique_ptr<CyclicResultInterface> GetCyclic(const std::optional<openpass::type::EntityId> entityId, const Key &key) const override;
    std::unique_ptr<AcyclicResultInterface> GetAcyclic(const std::optional<openpass::type::EntityId> entityId, const Key &key) const override;
    Values GetStatic(const Key &key) const override;
    Keys GetKeys(const Key &key) const override;

private:
    DataBufferBinding *dataBufferBinding = nullptr;
    DataBufferInterface *implementation = nullptr;
};

} // namespace core
