..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _documentation:

Documentation Concept
=====================

|Op_oss| is developed under the Eclipse Public License and as such private and commercial use is allowed under certain rules (see `EPL 2.0 <https://www.eclipse.org/legal/epl-2.0/>`_).
The basic documentation concept facilitates this by providing a way to include custom content which is not necessarily part of the |op_oss| distribution.
This results in certain restrictions on how documentation is to be written.
The following sections describe this restrictions and the process of integrating proprietary documentation into the |op_oss| documentation build.

Basic Build Mechanics
---------------------

The required steps to build the documentation are described in :ref:`download_and_install_openpass`, provided by CMake files using the build option ``WITH_GUI=ON``.
Before building, a temporary copy of the original documentation is made.
This temporary copy acts as *seam* for custom extension, as proprietary content is simply copied into the temporary folder (see below).
This mechanism keeps contents clearly separated during development and allows easy transition from closed to open source if desired.

References to files located outside of the documentation root is used at various places, as this allows to keep documentation as close to the source as possible.
These references normally would become invalid when the documentation source is copied or moved to another location.
Thus, placeholders are used to have a fixed reference to the |op_oss| tree:

  - *@*\ *OP_REL_ROOT*\ *@* - Root of repository (``.``)
  - *@*\ *OP_REL_GUI*\ *@* - Folder ``./gui``
  - *@*\ *OP_REL_SIM*\ *@* - Folder ``./sim``

This placeholders must be used when files outside of the documentation root shall be referenced.  
Note that this also makes sources more readable.

**Example**

.. literalinclude:: @OP_REL_ROOT@/doc/source/user_guide/configs/scenery.rst
   :start-at: OP_REL_SIM
   :lines: 1

.. warning:: 

   Generally, when moving or deleting files, make sure to run ``make clean`` prior to ``make doc`` to remove any outdated files from the build directory.

openPASS as Submodule
---------------------

#. **Write the documentation**

   As custom documentation simply integrates into the |op_oss| documentation, it is also written in the *reStructuredText* file format.
   Thereby files have to reside in a directory structure that is a clone of the open source documentation directory structure (starting from ``doc/source``).
   During the build your documentation and the open source documentation will both be copied to a temporary directory.

   .. note:: Files existing in both source directories will be overwritten at the destination, with the custom files having higher precedence.

   On the TOC tree level, the **seam** to custom files is made through *globbing* using wildcards, such as ``folder/*.rst``.
   Ordering of the files (and to avoid file name collisions) is established through a two digit number prefix and an underscore, e.g. ``10_quickstart.rst``.
   This allows injection of proprietary documentation at any position in the TOC tree, as the order of headings is determined by the (ASCII) sorting of the filenames.

#. **Referencing files**

   Sphinx uses some special path format when referencing files from documentation.
   All paths are relative to a single documentation root directory, but are specified like absolute paths (i.e. with a leading slash).
   Due to the documentation source files being copied to a temporary directory during build, all file references have to be prefixed with a path constant:

   - When specifying a file reference relative to the |op_oss| repository, the file path has to be prefixed with custom placeholders, such as *@*\ *OP_REL_ROOT*\ *@* (see above).
   - When referencing files relative to a custom root, additional placeholders can be introduced in a custom ``PrepareDocCustom.cmake`` (see next steps).
   - These placeholders will then coexist and will be replaced with the corresponding paths during build.

#. **Add a cmake file for documentation preparation**

   The file :download:`PrepareDocCustom.cmake<_static/custom_doc/PrepareDocCustom.cmake>` can be used as a template.
   Just add the placeholders used in your proprietary documentation.

   This diff highlights the important parts in comparison to the original ``PrepareDoc.cmake``, used in the open source documentation build:

   .. literalinclude:: _static/custom_doc/PrepareDocCustom.cmake
      :diff: @OP_REL_ROOT@/doc/PrepareDoc.cmake

#. **Add a ``doc`` CMake target to your custom build**

   To add your custom build target, the following ``CMakeLists.txt`` snippet can be used as template.
   Note the usage of the ``PrepareDocCustom.cmake``.

   .. literalinclude:: _static/custom_doc/CMakeLists_doc.cmake
      :emphasize-lines: 23

#. **Provide a config file for Sphinx**

   Sphinx allows to specify a configuration residing in ``conf.py`` in the documentation source directory.
   Customization is done by providing a customized file here, with the open source version as template (:download:`@OP_REL_ROOT@/doc/source/conf.py`).
