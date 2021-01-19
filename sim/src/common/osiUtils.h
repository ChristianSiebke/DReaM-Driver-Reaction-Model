/*******************************************************************************
* Copyright (c) 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include"osi3/osi_version.pb.h"

namespace osi3::utils
{

//! Writes the InterfaceVersion into a OSI message
template <typename Message>
[[maybe_unused]] static void SetVersion(Message& message)
{
  const auto currentInterfaceVersion = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);
  message.mutable_version()->CopyFrom(currentInterfaceVersion);
}

//! Writes the given Timestamp into a OSI message
template <typename Message>
[[maybe_unused]] static void SetTimestamp(Message& message, int timeInMs)
{
    message.mutable_timestamp()->set_seconds((timeInMs) / 1000);
    message.mutable_timestamp()->set_nanos(((timeInMs) % 1000) * 1e6);
}

}
