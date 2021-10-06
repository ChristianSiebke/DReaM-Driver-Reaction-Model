/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef IOPLUGININTERFACE_H
#define IOPLUGININTERFACE_H

#include "openPASS/PluginInterface.h"

class IOPluginInterface : public PluginInterface
{
public:
    ~IOPluginInterface() = default;

public:
    using Filepath = QString;

public:
    virtual Filepath getCurrentFilepath() const = 0;
    virtual void setCurrentFilepath(Filepath const &_filepath) = 0;

public:
    virtual bool save(Filepath const &_filepath) = 0;
    virtual bool load(Filepath const &_filepath) = 0;
    virtual bool clear() = 0;
};

Q_DECLARE_INTERFACE(IOPluginInterface, "openPASS.IOPluginInterface")

#endif // IOPLUGININTERFACE_H
