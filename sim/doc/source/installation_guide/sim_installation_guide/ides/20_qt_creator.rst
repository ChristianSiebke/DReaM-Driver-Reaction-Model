..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

Working with Qt Creator
=======================

Settings (MSYS2)
----------------

Setup Cmake-Kit
~~~~~~~~~~~~~~~

#. Add Cmake under ``Tools -> Options -> Kits -> Cmake``

   .. figure:: _static/images/qtcreator_settings_cmake.png
      :align: center
      :scale: 60%


#. Add C and C++ Compilers under ``Tools -> Options -> Kits -> Compilers``

   .. figure:: _static/images/qtcreator_settings_g++.png
      :align: center
      :scale: 60%


   .. figure:: _static/images/qtcreator_settings_gcc.png
      :align: center
      :scale: 60%


#. Add Debugger under ``Tools -> Options -> Kits -> Debuggers``

   .. figure:: _static/images/qtcreator_settings_gdb.png
      :align: center
      :scale: 60%


#. Add Kit under ``Tools -> Options -> Kits -> Kits``

   .. figure:: _static/images/qtcreator_settings_kit.png
      :align: center
      :scale: 60%


#. Adjust ``Cmake Generator`` under ``Tools -> Options -> Kits -> Kits`` to ``MinGW Makefiles``

   .. figure:: _static/images/qtcreator_settings_cmake_generator.png
      :align: center
      :scale: 60%

   .. note::
      
      Change the environment variable to ``MAKEFLAGS=-j4`` (or similar) to enable parallel building on the kit level.

Setup Project
~~~~~~~~~~~~~~

#. Load the project by opening a ``CMakeLists.txt`` file and configure it to use the new kit.

#. Setup missing Cmake flags (c.f. :ref:'Cmake')

   .. todo:: The configuration is still "itchy", as Qt creator changes the interface with every version

#. Override the make command under ``Project -> Build -> Build Steps`` to ``mingw32-make``

   .. figure:: _static/images/qtcreator_project_make.png
      :align: center
      :scale: 60%

   .. note::
      
      Unfortunatly, there seems to be no way to enable ``mingw32-make`` on the kit level.

Settings (Linux)
----------------

Setup Kit
~~~~~~~~~

Qt Creator should be able to come up with a suiteable kit for developing |op| on its own.
If not, check if all :ref:`prerequisites` are met. Also, the steps above should be a good indicator, where to look for potential troubles.

Setup Project
~~~~~~~~~~~~~~

#. Load the project by opening a ``CmakeLists.txt`` file and configure it to use a proper kit.

#. Setup missing Cmake flags (c.f. :ref:'Cmake')

   .. todo:: The configuration is still "itchy", as Qt creator changes the interface with every version

Alternative Method
------------------

.. admonition:: Version Issues
   
   Qt Creator is constantly improving the Cmake integration.
   Unfortunatly, some versions seem to be **buggy**.

   Tested Versions: 4.13.2 | 4.12.2

#. Follow the install instructions to invoke cmake from within your own build directory.
   - Execute the step for ``build-release``
   - Execute the step for ``build-debug``
#. Open Qt Creator
#. Open ``CMakeLists.txt``
#. Qt Creator should find the configured build directories and create a temporary kit

   .. figure:: _static/images/qtcreator_project_cmake.png
      :align: center
      :scale: 60%

   .. figure:: _static/images/qtcreator_project_loaded.png
      :align: center
      :scale: 60%

   .. figure:: _static/images/qtcreator_project_cmake_settings.png
      :align: center
      :scale: 60%