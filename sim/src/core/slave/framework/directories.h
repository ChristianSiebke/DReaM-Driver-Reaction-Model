/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>
#include <vector>

#include "common/opExport.h"

namespace openpass::core {

/// Directories takes care of a consistent handling of paths and directories
/// without making the underlying implementation public or a dependency
///
/// Once initialized, the paths are resolved w.r.t the applications base directory
/// if the paths are relative, or kept absolute
///
/// Note that all directories are returned without a trailing path seperator
/// Use Concat if you need to concat a path and a file with the current systems seperator
class CORESLAVEEXPORT Directories
{
public:
    Directories(const std::string& applicationDir,
                const std::string& libraryDir,
                const std::string& configurationDir,
                const std::string& outputDir);

    // This class should not be moved or assigned,
    // as there should be only one instance throughout the system
    Directories() = delete;
    Directories(const Directories&) = delete;
    Directories(Directories&&) = delete;
    Directories& operator=(Directories&&) = delete;

    const std::string baseDir;              ///!< path of the executed application
    const std::string configurationDir;     ///!< directory of the configuration files
    const std::string libraryDir;           ///!< directory of the libraries
    const std::string outputDir;            ///!< directory for outputs

    /// \brief  Concats a path and a file with the seperator used by the current system
    /// \param  path     e.g. /the_path
    /// \param  file     e.g. the_file
    /// \return Concatenated string, e.g. /the_path/the_file
    ///
    static const std::string Concat(const std::string& path, const std::string& file);

    /// \brief  Concats a path and multiple filenames with the seperator used by the current system
    /// \param  path            e.g. /the_path
    /// \param  filenames     e.g. the_filenames
    /// \return Vector of concatenated strings, e.g. /the_path/the_filename
    ///
    static const std::vector<std::string> Concat(const std::string& path, const std::vector<std::string>& filenames);

    /// \brief  Removes the file from a path
    /// \param  path with file
    /// \return Resolved string  e.g. /baseDir/path or /path, respectively
    static const std::string StripFile(const std::string& path);

    /// \brief  Checkes whether a given path is absolute or relative
    /// \param  path
    /// \return true if relative path
    static bool IsRelative(const std::string& path);

private:
    /// \brief  Internally used to get a cononical path w.r.t to the application path
    /// \param  applicationPath  absolute path of the executed file, e.g. /baseDir
    /// \param  path             relative or absolute path, e.g. path or /path
    /// \return Resolved string  e.g. /baseDir/path or /path, respectively
    ///
    static const std::string Resolve(const std::string& applicationPath, const std::string& path);
};

} // openpass::core
