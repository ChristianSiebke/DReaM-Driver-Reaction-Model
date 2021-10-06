..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH
                2021 BMW AG

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _cmake:

CMake Variables and Options
===========================

|Op| uses CMake as default cross-platform build environment.
CMake varibles describe configuration settings, which can be overriden by the user. 
To override any build configuration, the CMake variables have to be provided when CMake gets run.

If available, recommended options are shown in **bold**.

The following guide is structered as follows: The first part describes the available variables and options.
There are standard CMake variables which are marked with the prefix ``CMAKE_`` and other defined variables 
used internally to further specify the build of |op|. As the result of a proper CMake call is a Makefile, 
which then acts as base for the actual build using ``make``, the second part describes best practices around the make commands using ``make``.

Note, this guide approaches CMake from the *command line perspective*. 
Adjustments for VSCode and QtCreator can be found under :ref:`vscode` and :ref:`qtcreator`.

.. admonition:: See also

   CMake Documentation: https://cmake.org/cmake/help/latest/index.html

CMake Generator
---------------

This is only important for **Windows**.
To generate MSYS compatible makefiles use ``-G "MSYS Makefiles"`` (c.f. :ref:`MSYS2 <msys2>`).

.. _cmake_prefix_path:

CMAKE_PREFIX_PATH
-----------------

This variable makes the prerequisites available to |op| as semicolon-separated list of directories, 
specifying installation prefixes to be searched by cmake.
Please refer to :ref:`Prerequisites` for the list of mandatory packages and libraries.
CMake will issue an error, if one prerequisite is missing.

Generally, cmake recognizes installed libraries (e.g. googletest) on its own.
By setting an explicit ``CMAKE_PREFIX_PATH`` for a library, it is possible to override the system libraries.
Use this, when an exact library version, or a customized library shall be used.

.. note::
   
   In the following example, non-standard libraries are assumed to be in the folder ``deps/thirdParty``.

**Example** (system libraries **not** set):

.. tabs::

   .. tab:: Windows

      .. code-block:: bash

         cmake -G "MSYS Makefiles"
               -D CMAKE_PREFIX_PATH="\
                  /mingw64/bin;\
                  $PWD/../deps/thirdParty/win64/FMILibrary;\
                  $PWD/../deps/thirdParty/win64/osi;\
                  $PWD/../deps/thirdParty/win64/minizip;\
               -D <other variables>\
               ..

   .. tab:: Linux

      .. code-block:: bash

         cmake -D CMAKE_PREFIX_PATH=\
                  $PWD/../deps/thirdParty/linux64/FMILibrary\;\
                  $PWD/../deps/thirdParty/linux64/osi\;\
                  $PWD/../deps/thirdParty/linux64/minizip\;\
               -D <other variables> \
               ..

      .. warning:: The semicolon ``;`` needs to be escaped (``\;``) unless the string is quoted.

.. note:: Please also read through :ref:`cmake_openpass_variables` for further useful hints, e.g. why explicitly setting the MinGW path might be a necessary in some situations.

Installation directory

CMAKE_INSTALL_PREFIX
--------------------

- Install directory used by install, when invoking ``make install``
- Recommendation: ``/OpenPASS/bin/core`` (Linux) | ``C:/OpenPASS/bin/core`` (Windows)

CMAKE_WITH_DEBUG_POSTIX
-----------------------

- Used only in conjunction with *Visual Studio Debug Builds* (MSVC), as it distinguishes release/debug DLLs by a postfix ``d``.
- Options: **OFF** | ON

CMAKE_BUILD_TYPE
----------------

- Specifies the build type on single-configuration generators.
- Typical: Release | Debug
- See: https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html

.. note::

   IDEs, as Qt Creator, might set this variable base on the current build type on their own.

CMAKE_C_COMPILER
----------------

- Options: **gcc-10** | gcc-9 | gcc-8
- See: https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER.html

.. note::

    IDEs, as Qt Creator, might set this variable via *kit* settings.

CMAKE_CXX_COMPILER
------------------

- Options: **g++-10** | g++-9 | g++-8
- See: https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER.html

.. note::

    IDEs, as Qt Creator, might set this variable via *kit* settings.

CMAKE_OBJECT_PATH_MAX
---------------------

- Under windows, errors from too long paths could be prevented by setting this value to 255 (maximum).
- See: https://cmake.org/cmake/help/latest/variable/CMAKE_OBJECT_PATH_MAX.html

.. _cmake_openpass_variables:

USE_CCACHE
----------

- Activates ccache (see :ref:`prerequisites_ccache`)
- Options: **ON** | OFF

WITH_SIMCORE
------------

- Build OSI based scenario simulation, also know as openPASS core (slave).
- Options: OFF | **ON**

WITH_DOC
--------

- Build sphinx based documentation
- Options: OFF | **ON**

WITH_API_DOC
------------

- Build sphinx based developer documentation
- Options: **OFF** | ON

.. note:: Automatically activates ``WITH_DOC`` if ON

.. warning:: Building the API doc takes pretty long.

WITH_COVERAGE
-------------

- Add test targets for code coverage analysis (lcov) and HTML report generation (genhtml)
- Options: **OFF** | ON
- Use ``COVERAGE_EXCLUDE`` to remove folders from the analysis

.. note:: 

   The generated files are placed next to the test executable.
   Each test will be built without optimization, which will increase the testing run-time.
   
WITH_GUI
--------

- Activates the additional build of the GUI provided with |Op| (open source).
- Options: **OFF** | ON

.. note::

   Please refer to :ref:`gui_user_guide` for information on the GUI.

WITH_TESTS
----------

- Build unit tests
- Options: OFF | **ON**

WITH_ENDTOEND_TESTS
-------------------

- Enables execution of end to end tests using :ref:`testing_endtoend`.
- Tests can be ran by executing ``make install && make pyOpenPASS``
- Options: **OFF** | ON

OPENPASS_ADJUST_OUTPUT
----------------------

- Adjusts if builds are executed in the (CMake default) folder ``build`` or directly in the specified install directory.
  Latter mimics the former qmake behavior let you skip the call ``make install``.
- Options: **OFF** | ON

.. warning::

   When skipping ``make install``, dependencies are not copied into the output folder, which could cause crashes due to missing or outdated libraries.

.. _cmake_protobuf_arenas:

WITH_EXTENDED_OSI
-----------------

- When set, assumes that extended version of OSI is available, by enabling the ``USE_EXTENDED_OSI`` preprocessor variable.
- This variable can be used to enable e.g. customized OSI features:

   .. code-block:: c++

      #ifdef USE_EXTENDED OSI
      #include "osi3/osi_<custom_message>.pb.h"
      #endif

- Options: **OFF** | ON

WITH_PROTOBUF_ARENA
-------------------

- | *Arena allocation is a C++-only feature that helps you optimize your memory usage and improve performance when working with protocol buffers.*
  | (from https://developers.google.com/protocol-buffers/docs/reference/arenas)
- Options: **ON** | OFF

.. note::

   This feature is only available, if protobuf related libraries are also compiled with arenas (see :ref:`building_osi`).
   Fortunately, the implementation falls back to regular allocation if not, which simply results in less performance.

INSTALL_SYSTEM_RUNTIME_DEPS
---------------------------

- during installation step, this configuration copies detected system runtime dependencies to install directory (i.e. MinGW system libraries)
- Options: ON | **OFF**

.. warning:: 
   
   Under windows, automatic resolution might fail if other MinGW instances are installed.
   As several programs use MinGW under the hood, it is recommended to set the used MinGW path in the CMAKE_PREFIX_PATH  explicitly:
   
      .. code-block:: bash
      
         CMAKE_PREFIX_PATH = mingw64/bin;\...

INSTALL_EXTRA_RUNTIME_DEPS
--------------------------

- during installation step, this configuration copies detected runtime dependencies (i.e. required shared libraries) specified in `CMAKE_PREFIX_PATH` to install directory
- Options: ON | **OFF**

Make Targets/Commands
---------------------

|Op| defines build targets by major modules or components, such as ``OpenPassSlave`` or ``Algorithm_FmuWrapper``.
After calling CMake, simply build |op| by calling ``make``.

.. admonition:: See also

   https://www.tutorialspoint.com/unix_commands/make.htm

Build and Install
~~~~~~~~~~~~~~~~~

- ``make``

- ``make install``

  .. warning:

     - Right now, there is still an issue with an additinal ``bin`` folder.
       After installing, the content of the `./bin` folder needs to be copied into `.`.

     - Make install seems to have troubles on some systems when copying the dependencies.
      Check if the libraries of e.g. OSI are copied into the execution directory of the core.

     - MinGW system libraries are not a dependency visible to make.

- ``make <target>``: Build a single target

Executing Tests
~~~~~~~~~~~~~~~

- All tests: ``make test ARGS="--output-on-failure -j3"``
- Single test: ``make test OpenPassSlave_Tests ARGS="--output-on-failure -j3"``
