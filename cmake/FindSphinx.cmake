################################################################################
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################
#
# Find Package Adapter for "Sphinx"
#
# @see http://sphinx-doc.org/
#
# - SPHINX_EXECUTABLE: The executable
# - SPHINX_FOUND: True if Sphinx was found
#
include(FindPackageHandleStandardArgs)
find_program(SPHINX_EXECUTABLE sphinx-build "Path to the sphinx-build executable")
find_package_handle_standard_args(Sphinx "Unable to locate sphinx-build" SPHINX_EXECUTABLE)