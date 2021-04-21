..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH
                2021 BMW AG

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _building prerequisites:

Build and Install Prerequisites
===============================

This section describes how to compile prerequisites of |op| using source packages.

To keep this guide as slim as possible, it only describes the **Windows perspective**.
In general, this should work for Linux as well by adjusting paths and the ``make`` command accordingly.
  
.. note::
   
   If you are unfamiliar to ``CMake`` or working within a |mingw_shell|, Section :ref:`cmake` and :ref:`msys2` might give you a short introduction on these topics in the scope of building |op| itself.
   
Location Of Installed Source Packages
-----------------------------------------------

The goal of this section is to download, build and install
all necessary prerequisites into a suitable directory which later will be copied into the repository in order to resolve third party dependency.
The following directory tree shows the folder structure, which will be created by following the recommendations of this guide. 

.. code-block:: bash

   C:\OpenPASS\thirdParty
   ├── FMILibrary
   │   ├── include
   │   └── lib
   └── osi
       ├── include
       └── lib 

On the basis of this structure, we will explain further steps.

In the folder structure above:

- ``C:\OpenPASS\thirdParty`` refers to a temporary directory used to built the prerequisites from source, **not** the ``simopenpass`` repository
- ``FMILibrary`` is the install directory of the ``Functional Mock-up Interface (FMI)`` when build from source
- ``osi`` is the install directory of the ``Open Simulation Interface (OSI)`` when build from source.

.. _building_osi:

Building OSI
------------

As teased in :ref:`Prerequisites`, the core component ``World_OSI`` uses :term:`OSI` as backend storage.
OSI itself uses ``protobuf`` to describe data structures in a platform independent way by means of `*.proto` files.
When building OSI, these files are converted into C++ headers and sources, using the protobuf compiler ``protoc``.
Finally, the sources are then compiled into a library.
|Op| finally uses the library and the generated headers to interface the library.

#. Open |mingw_shell| and create directory structure

   .. code-block:: bash

      cd /C/
      mkdir -p OpenPASS/thirdParty/sources

#. Download release 3.2.0 from https://github.com/OpenSimulationInterface/open-simulation-interface 

#. Extract to ``C:\OpenPASS\thirdParty\sources\open-simulation-interface-3.2.0``

#. Navigate to the extracted folder

   .. code-block:: bash

      cd /C/OpenPASS/thirdParty/sources/open-simulation-interface-3.2.0

#. Optional: Enable Arenas
   
   For better performance, |op| supports protobuf Arenas allocation (https://developers.google.com/protocol-buffers/docs/reference/arenas).
   To use this feature, OSI and |op| needs to be compiled with Arenas support.
   See :ref:`cmake_protobuf_arenas` how this feature is enabled in |op|.

   To enable Arenas support for OSI, the line ``option cc_enable_arenas = true;`` needs to be added **manually** to all OSI proto files before compilation.
   This can be achieved in two ways. Either the line ``option cc_enable_arenas = true;`` gets added manually **after the second line** of each PROTO file in ``C:\OpenPASS\thirdParty\sources\open-simulation-interface`` by using a text editor or one makes use of the stream editor in the |mingw_shell|:

   .. code-block:: bash

      find . -maxdepth 1 -name '*.proto' -exec sed -i '2i option cc_enable_arenas = true;' {} \;

   .. warning:: 

      The first line of each OSI proto file specifies the protobuf syntax used. 
      If Arenas support is added before the syntax specification, errors occur.

   If everything goes well, ``protoc`` will generate all the magic necessary, otherwise issues an error during compilation.



#. Create build directory

   .. code-block:: bash

      mkdir build
      cd build

#. Run Cmake

   .. code-block:: bash

      cmake -G "MinGW Makefiles" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty/osi \
            ..

   .. note:: 
   
      If protobuf is used as custom build (see :ref:`building_protobuf`),
      additional ``CMake`` variables have to be set.

      .. code-block:: bash

         cmake -G “MinGW Makefiles” \
               -DCMAKE_BUILD_TYPE=Release \
               -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty/osi \
               -DProtobuf_INCLUDE_DIR=C:/OpenPASS/thirdParty/protobuf/include \
               -DProtobuf_PROTOC_EXECUTABLE=C:/OpenPASS/thirdParty/protobuf/bin/protoc.exe \
               -DProtobuf_LIBRARIES=C:/OpenPASS/thirdParty/protobuf/lib  \
               ..


      Moreover, linker flags for protobuf have to be added. 
      Please note that OSI does not allow to hook in a custom protobuf library.
      To force compilation against a custom library, edit ``open-simulation-interface-3.2.0\build\CMakeFiles\open_simulation_interface.dir\linklibs.rsp``
      and add ``-LC:/OpenPASS/thirdParty/protobuf/lib -lprotobuf`` to the end of the line by using a text editor or make use of the folowwing command within the |mingw_shell|:.

      .. code-block:: bash

         echo -n " -LC:/OpenPASS/thirdParty/protobuf/lib -lprotobuf" >> /C/OpenPASS/thirdParty/sources/open-simulation-interface-3.2.0/build/CMakeFiles/open_simulation_interface.dir/linklibs.rsp
         
      .. admonition:: A little bit hacky...
      
         If anybody knows how to avoid this step, please let us know.

#. Compile
   
   .. code-block:: bash

      mingw32-make -j3

#. Install

   .. code-block:: bash

      mingw32-make install

   
.. admonition:: Documentation
   
   The OSI class documentation is part of the source code and can be compiled using Doxygen.
   Instructions are located in the OSI ``Readme.md``. A pre-compiled version is located `here <https://opensimulationinterface.github.io/open-simulation-interface/index.html>`_. 
   
   So far, the documentation does not include the extensions from the openpass-trafficAgents branch.


.. _building_fmil:

Building FMIL
-------------

#. Download release 2.0.3 from https://github.com/modelon-community/fmi-library

#. Extract to ``C:\OpenPASS\thirdParty\sources\fmi-library-2.0.3``

#. Open |mingw_shell| and navigate to the extracted folder

   .. code-block:: bash 

      cd /C/OpenPASS/thirdParty/sources/fmi-library-2.0.3

#. Create build directory

   .. code-block:: bash

      mkdir build
      cd build

#. Run Cmake

   .. code-block:: bash

      cmake -G "MinGW Makefiles" \
            -DFMILIB_INSTALL_PREFIX=C:/OpenPASS/thirdParty/FMILibrary \
            -DCMAKE_BUILD_TYPE=Release \
            -DFMILIB_BUILD_STATIC_LIB=OFF \
            -DFMILIB_BUILD_SHARED_LIB=ON  \
            ..

#. Apply Patch
   
   As FMIL and the internally used `FMU Compliance Checker <https://github.com/modelica-tools/FMUComplianceChecker>`_ has issues with loading and private entry points, the following patch needs to be applied: 
   
   - :download:`Windows/Linux Patch </_static/resources/fmil/fmil203.patch>`

#. Compile

   .. code-block:: bash

      mingw32-make -j3
  
#. Install

   .. code-block:: bash
  
      mingw32-make install


.. _building_protobuf:

Optional: Custom Protobuf Build
-----------------

As Protobuf can be installed easily as binary package in different versions (see :ref:`Binary_packages`) via appropiate packaging managing systems, there is no need to build protobuf from source.
However, if a custom build is necessary, this section gives instructions, how to compile version 3.11.4. and hook it into the |op| build.
Adjust this guide to your needs.

#. Download release 3.11.4 from https://github.com/protocolbuffers/protobuf/releases

#. Extract to e.g. ``C:\OpenPASS\thirdParty\sources\protobuf-cpp-3.11.4``

#. Open |mingw_shell| and navigate to the extracted folder

   .. code-block:: bash

      cd /C/OpenPASS/thirdParty/sources/protobuf-cpp-3.11.4

#. Create build directory

   .. code-block:: bash
      
      cd cmake
      mkdir build
      cd build

#. Run Cmake

   .. code-block:: bash
      :emphasize-lines: 5

      cmake -G "MinGW Makefiles" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty/protobuf \
            -Dprotobuf_BUILD_SHARED_LIBS=ON \
            -Dprotobuf_BUILD_TESTS=OFF  \
            ..

   .. note::
   
      Tests are disabled due to compiler warnings treated as errors (may vary with compiler version).

#. Compile

   .. code-block:: bash

      mingw32-make -j3
   
#. Install

   .. code-block:: bash
   
      mingw32-make install
   
.. note::

   Please refer to :ref:`runmingwexe` to see how to run the protobuf compiler ``protoc.exe`` outside of the |mingw_shell|.


Deprecated: qmake build
-----------------------

.. warning:: 

   The following information is **DEPRECATED** and only needed if a ``qmake`` build shall be invoked.

Historically, |op_oss| uses a **single entry-point** for libraries and headers, so all prerequisites had to be located within a common folder structure:

.. code-block:: bash
   
   C:\OpenPASS\thirdParty
   ├── include
   │   ├── prerequisite_1.h
   │   ├── prerequisite_2.h
   │   └── ...
   └── lib
         ├── prerequisite_1.dll
         ├── prerequisite_2.dll
         └── ...

If ``qmake`` needs to be invoked, and more than one prerequisite is customized, **a manual step** is necessary to establish this structure.

Example:

#. Build prerequisite_1
#. Build prerequisite_2
#. Copy libraries of both prerequisites into e.g. ``C:\OpenPASS\thirdParty\lib``
#. Copy common headers into e.g. ``C:\OpenPASS\thirdParty\include``
#. Before compiling |op|: Reference the entry points by adding the following arguments to the qmake command

   .. code-block:: batch

      EXTRA_INCLUDE_PATH=C:\OpenPASS\thirdParty\include
      EXTRA_LIB_PATH=C:\OpenPASS\thirdParty\lib
      
   Make sure that there is no linebreak between the two arguments.
   Qmake cannot handle this, but does not complain.