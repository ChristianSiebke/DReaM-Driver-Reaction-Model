################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

# The rst files of the sphinx documentation contain PLACEHOLDERS for referencing
# files outside the source folder of the documentation. This makes references
# into the repository "as a whole" easier, but also introduces a step for
# resolving these references before building. To prevent changes in the original
# documentation source, the this file copies the original source folder ${SRC}
# into a new ${DST} folder. After that, the PLACEHOLDERS are updated w.r.t
# to the the new origin.
#
# Currently supported PLACEHOLDERS
# - @OP_REL_SIM@ => relative path to the "op sim" root (e.g. deps/os/sim if
#   using a git submodule with the openPASS open source code at deps/os inside
#   the local repository)
# - @CUSTOM_REL_SIM@ => relative path to the "custom sim" root (.)

macro(copy_documentation source destination)
    message(VERBOSE "Copy ${source} to ${destination}")
    file(COPY ${source} DESTINATION ${destination})
endmacro()

macro(update_placeholder source destination)
    message(VERBOSE "Updating ${source} to ${destination}")
    file(RELATIVE_PATH target ${destination} ${source}/../..)

    # Remove potential trailing "/"
    string(REGEX REPLACE "(.*)/$" "\\1" target ${target})

    # Placeholder for conf.py: no initial '/' => real relative paths
    set(OP_REL_SIM ../${target}/deps/os/sim)   # relative path to the openPASS open source code, with prefix '../${target}' pointing to the custom repository root if this file is located at <root>/doc
    set(CUSTOM_REL_SIM ../${target})           # relative path to the custom repository root (equal to custom sim root in this example)

    configure_file(${destination}/source/conf.py
                   ${destination}/source/conf.py @ONLY)

    # Placeholder for RST files: use initial '/' => sphinx style for "from source"
    # Override old one, because we want to use the same placeholder in both contexts
    set(OP_REL_SIM /${OP_REL_SIM})
    set(CUSTOM_REL_SIM /${CUSTOM_REL_SIM})

    file(GLOB_RECURSE rstFiles LIST_DIRECTORIES false ${destination}/*.rst)

    foreach(rstFile IN LISTS rstFiles)
        message(DEBUG "Replacing placeholders in ${rstFile}")
        configure_file(${rstFile} ${rstFile} @ONLY)
    endforeach()
endmacro()

copy_documentation(${SRC} ${DST})
update_placeholder(${SRC} ${DST})
