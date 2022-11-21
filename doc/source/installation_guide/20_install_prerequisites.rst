..
  *******************************************************************************
  Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
                2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. |win_only| image:: /_static/win_only.png

.. _prerequisites:

Installing Prerequisites
========================

Before being able to compile and run |op|, make sure to have all dependencies installed. 
The third party software dependency of |op| can be divided into:

- **Build environment**, which manages third party software and is used to compile and install |op|
- **Binary packages**, which can be easily installed via a package manager of the respective build environment
- **Source packages**, which need to be built from source code

This section gives detailed information about the prerequisites and tested version.

.. _building_under_windows:

Installing the Build Environment
--------------------------------

.. tabs::
   
   .. tab:: Windows

      #. MSYS2
      
         On Windows, the build environment of choice are :term:`MSYS2` programming tools.
         :term:`MSYS2` is used to install some third-party software on which |op| depends. 
         Also, the unix-like shell simplifies c++ compilation on Windows.
         For details, see `MSYS2 website <https://www.msys2.org/>`_.

         
         .. _msys2:

      #. Download MSYS2

         The latest 64-bit packages are located at https://repo.msys2.org/distrib/x86_64/. 
         Download a non-base package, i.e. `msys2-x86_64-20200903.exe <https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20200903.exe>`_


         .. _msys2_installation:

      #. Install MSYS2

         Run the downloaded executable and adjust suggested settings to your needs (defaults are fine).
         In the following, it is assumed that MSYS2 is installed under ``C:\msys64``.

      #. Understand the Build Environment

         MSYS2 provides three different environments, located in the MSYS2 installation directory:

         .. _fig_msys2_environments:

         .. figure:: _static/images/msys2.png
            :align: center
            :alt: MSYS2 Apps

         MSYS2 Environments

         - MSYS2 MSYS: Common environment, i.e. for package management
         - MSYS2 MinGW 32-bit: A MinGW 32-bit environment
         - MSYS2 MinGW 64-bit: A MinGW 64-bit environment

         .. warning::

            | MSYS2 MinGW 64-bit is **the**  |op| development environment and will be referred to as |mingw_shell|.

   .. _building_under_linux:

   .. tab:: Linux

      On Linux, no special build environment is needed. |Op| is developed under Debian 64-Bit, 
      which means that developing under a recent Ubuntu distribution will also work.
      Debian Bullseye or Ubuntu 20.10 is recommended. Debian uses ``apt`` (or ``apt-get``) as package managing system. 
      Details will be given in :ref:`binary_packages` and :ref:`building_prerequisites`. 


.. _binary_packages:

Installing the Binary Packages
------------------------------

The first set of dependencies we need to install in order to successfully compile |op| are the binary packages. These can be installed via appropiate package manager. 

.. tabs::
   
   .. tab:: Windows (MSYS2)

      #. Open ``MSYS2 MSYS`` and execute the following package manager ``pacman`` commands to update the package repository and upgrade system packages:

         .. code-block:: 

            pacman -Syuu
      
         If the upgrade requires a restart of MSYS2, resume the upgrade by re-opening the shell and call:

         .. code-block:: 
         
            pacman -Suu

      #. Required packages (can be specified in single command line if desired):

         .. literalinclude:: _static/msys2_packages.txt
            :language: bash

         .. admonition:: Versions
            
            | MSYS2 provides rolling release versions, so some packages might be too "up-to-date".
            | Tested packages - ate time of writing - have been listed above as comment.
            | If in doubt, download the package in the right version from the `MSYS2 package repository <http://repo.msys2.org/mingw/x86_64/>`_.
            | Install with ``pacman -U <package-filename>``

      #. Optional Packages

         .. code-block:: 

            pacman -S git
            pacman -S diffutils
            pacman -S patch
            pacman -S dos2unix
            pacman -S mingw-w64-x86_64-ag
            pacman -S mingw-w64-x86_64-qt5-debug
            pacman -S zlib-devel

      .. admonition:: GIT/SSH
      
         The |mingw_shell| does not access an already existing git installation or available SSH keys.
         Make sure, to update/copy your configuration and credentials within the |mingw_shell| before working with git.   

   .. tab:: Linux (Debian Bullseye)

      #. Update  the package database on the system

         .. code-block:: 

            apt update

      #. Upgrade existing software to latest version

         .. code-block:: 

            apt upgrade

      #. Install required binary packages

         .. code-block:: 

            # for simulator
            apt install ccache
            apt install cmake
            apt install doxygen
            apt install googletest
            apt install gcc
            apt install g++
            apt install graphviz
            apt install libprotobuf-dev
            apt install protobuf-compiler  # when building osi
            apt install qt5-default
            apt install zlib1g-dev

            # for documentation
            sudo apt install doxygen python3 python3-pip libenchant-2-2 dvipng
            pip3 install sphinx sphinx-rtd-theme sphinx-tabs breathe exhale sphinxcontrib-spelling

         Under Linux, it is deliberate that the googletest package only installs the header files to the system, 
         but not the static and dynamic libraries. The missing libraries can be build and installed to ``/usr/lib`` via

         .. code-block:: 

            cd /usr/src/googletest
            cmake .
            make
            make install


**EndToEnd Test Framework**

If the :ref:`testing_endtoend` shall be used, additional requirements have to be considered.

The test framework is based on Python and some additional Python modules.
Installation of the required modules can be accomplished using ``pip``.
Please refer to the file ``requirements.txt`` located in ``sim/tests/endToEndTests/pyOpenPASS`` for a list of dependencies.

The requirements file can be directly passed to ``pip``:

.. code-block::

   pip install -r requirements.txt

(executed from ``sim/src/tests/endToEndTests/pyOpenPASS``)

.. warning::
   |win_only|
   ``pip install`` will try to fetch precompiled packages by default.
   If it is unable to locate a binary package for the current environment, packages will be compiled from source.
   This step fails for the ``numpy`` package when being built from the MSYS2 environment.
   Thus, it is recommended to set up a native Windows Python environment and perform the installation there.
   To force the usage of a specific Python environment, the variable ``Python3_EXECUTABLE`` can be set to the indended Python interpreter executable during cmake configuration (see :ref:`download_and_install_openpass`).

Please refer to :ref:`testing_endtoend` for more details for using the framework.


.. _building_prerequisites:

Installing the Source Packages
------------------------------

This section describes how to compile prerequisites of |op| using source packages.
  
.. note::
   
   If you are unfamiliar to ``CMake`` or working within a |mingw_shell|, Section :ref:`cmake` and :ref:`MSYS2 <msys2>` might give you a short introduction on these topics in the scope of building |op| itself.
   
Location Of Installed Source Packages
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The goal of this section is to download necessary source packages and install
them into a suitable directory. This directory will later on be copied into the |op| repository in order to resolve third party dependency.
The following directory tree shows the folder structure, which will be created by following the recommendations of this guide. 

.. tabs::

   .. tab:: Windows
         
      ::

         C:\OpenPASS\thirdParty
         ├── FMILibrary
         │   ├── include
         │   └── lib
         └── osi
            ├── include
            └── lib 

      In the folder structure above:

      - ``C:\OpenPASS\thirdParty`` refers to a temporary directory used to built the prerequisites from source, **not** the ``simopenpass`` repository
      - ``FMILibrary`` is the install directory of the ``Functional Mock-up Interface (FMI)`` when build from source
      - ``osi`` is the install directory of the ``Open Simulation Interface (OSI)`` when build from source.

   .. tab:: Linux
         
      ::

         ~/OpenPASS/thirdParty
         ├── FMILibrary
         │   ├── include
         │   └── lib
         └── osi
            ├── include
            └── lib 

      In the folder structure above:

      - ``~/OpenPASS/thirdParty`` refers to a temporary directory used to built the prerequisites from source, **not** the ``simopenpass`` repository
      - ``FMILibrary`` is the install directory of the ``Functional Mock-up Interface (FMI)`` when build from source
      - ``osi`` is the install directory of the ``Open Simulation Interface (OSI)`` when build from source.

On the basis of this structure, we will explain further steps.

.. _building_osi:

Build and Install OSI
~~~~~~~~~~~~~~~~~~~~~

As can be looked up in :ref:`Prerequisites_osi`, the core component ``World_OSI`` uses :term:`OSI` as backend storage.
OSI itself uses ``protobuf`` to describe data structures in a platform independent way by means of `*.proto` files.
When building OSI, these files are converted into C++ headers and sources, using the protobuf compiler ``protoc``.
Finally, the sources are then compiled into a library.
|Op| finally uses the library and the generated headers to interface the library.

#. Open and create directory structure

   .. tabs::

      .. tab:: Windows

         Start |mingw_shell|

         .. code-block::

            cd /C/
            mkdir -p OpenPASS/thirdParty/sources

      .. tab:: Linux

         Start ``Bash`` shell

         .. code-block::

            cd ~
            mkdir -p OpenPASS/thirdParty/sources

#. Download release 3.2.0 from https://github.com/OpenSimulationInterface/open-simulation-interface 

#. Extract

   - for Windows to ``C:\OpenPASS\thirdParty\sources\open-simulation-interface-3.2.0``

   - for Linux to ``~/OpenPASS/thirdParty/sources/open-simulation-interface-3.2.0``

#. Navigate to the extracted folder

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cd /C/OpenPASS/thirdParty/sources/open-simulation-interface-3.2.0

      .. tab:: Linux

         .. code-block:: 
      
               cd ~/OpenPASS/thirdParty/sources/open-simulation-interface-3.2.0

#. Optional: Enable Arenas
   
   For better performance, |op| supports protobuf Arenas allocation (https://developers.google.com/protocol-buffers/docs/reference/arenas).
   To use this feature, OSI and |op| needs to be compiled with Arenas support.
   See :ref:`cmake_protobuf_arenas` how this feature is enabled in |op|.

   To enable Arenas support for OSI, the line ``option cc_enable_arenas = true;`` needs to be added **manually** to all OSI proto files before compilation.
   
   This can be achieved in two ways. Either the line ``option cc_enable_arenas = true;`` gets added manually **after the second line** of each PROTO file 
   
   - for Windows in ``C:\OpenPASS\thirdParty\sources\open-simulation-interface`` 
   - for Linux in ``~/OpenPASS/thirdParty/sources/open-simulation-interface`` 
  
   by using a text editor or one makes use of the stream editor in the shell:

   .. code-block:: 

      find . -maxdepth 1 -name '*.proto' -exec sed -i '2i option cc_enable_arenas = true;' {} \;

   .. warning:: 

      The first line of each OSI proto file specifies the protobuf syntax used. 
      If Arenas support is added before the syntax specification, errors occur.

#. Create build directory

   .. code-block:: 

      mkdir build
      cd build

#. Run Cmake

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cmake -G "MSYS Makefiles" \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty/osi \
                  ..

      .. tab:: Linux

         .. code-block:: 
         
            cmake -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX=~/OpenPASS/thirdParty/osi \
                  ..

#. Compile

   .. code-block:: 

         make -j3

   .. warning::

      If ``protobuf`` compiler complains, do 

      .. code-block:: 

         protoc --version

      and check if correct protobuf version is used.
   
#. Install
         
   .. code-block:: 

      make install
      
   .. admonition:: Documentation
      
      The OSI class documentation is part of the source code and can be compiled using Doxygen.
      Instructions are located in the OSI ``Readme.md``. A pre-compiled version is located `here <https://opensimulationinterface.github.io/open-simulation-interface/index.html>`_. 
      
      So far, the documentation does not include the extensions from the openpass-trafficAgents branch.


.. _building_fmil:

Build and Install FMIL
~~~~~~~~~~~~~~~~~~~~~~

#. Download release 2.0.3 from https://github.com/modelon-community/fmi-library

#. Extract

   - for Windows to ``C:\OpenPASS\thirdParty\sources\fmi-library-2.0.3``

   - for Linux to ``~/OpenPASS/thirdParty/sources/fmi-library-2.0.3``

#. Navigate to the extracted folder

   .. tabs::

      .. tab:: Windows

         Start |mingw_shell|

         .. code-block:: 

            cd /C/OpenPASS/thirdParty/sources/fmi-library-2.0.3

      .. tab:: Linux

         Start ``Bash`` shell

         .. code-block:: 
      
            cd ~/OpenPASS/thirdParty/sources/fmi-library-2.0.3

#. Create build directory

   .. code-block:: 

      mkdir build
      cd build

#. Run Cmake

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cmake -G "MSYS Makefiles" \
                  -DFMILIB_INSTALL_PREFIX=C:/OpenPASS/thirdParty/FMILibrary \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DFMILIB_BUILD_STATIC_LIB=OFF \
                  -DFMILIB_BUILD_SHARED_LIB=ON  \
                  ..

      .. tab:: Linux

         .. code-block:: 

            cmake -DFMILIB_INSTALL_PREFIX=~/OpenPASS/thirdParty/FMILibrary \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DFMILIB_BUILD_STATIC_LIB=OFF \
                  -DFMILIB_BUILD_SHARED_LIB=ON  \
                  ..

#. Leave build directory

   .. code-block:: 

      cd ..
               
#. Apply Patch
   
   As FMIL and the internally used `FMU Compliance Checker <https://github.com/modelica-tools/FMUComplianceChecker>`_ has issues with loading and private entry points, the following patch needs to be applied: 
   
   - :download:`Windows/Linux Patch </_static/resources/fmil/fmi-library-2.0.3-fixes.patch>`

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            patch -l -p1 "<path/to>/fmi-library-2.0.3-fixes.patch"

      .. tab:: Linux

         .. code-block:: 

            dos2unix src/Import/src/FMI1/fmi1_import_capi.c src/Import/src/FMI2/fmi2_import_capi.c src/Util/include/JM/jm_portability.h
            patch -l -p1 "<path/to>/fmi-library-2.0.3-fixes.patch"

#. Enter build directory

   .. code-block:: 

      cd build

#. Compile

   .. code-block:: 

      make -j3

#. Install

   .. code-block:: 

      make install


