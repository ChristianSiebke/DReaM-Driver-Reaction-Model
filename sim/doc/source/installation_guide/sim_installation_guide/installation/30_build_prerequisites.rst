..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

Building Prerequisites
======================

This section describes custom building of prerequisites of |op|.

To keep this guide as slim as possible, it only describes the **Windows perspective**.
In general, this should work for Linux as well by adjusting paths and the ``make`` command accordingly.
  
.. note::
   
   If you are unfamiliar to ``Cmake`` or working within a |mingw_shell|, Section :ref:`cmake` and :ref:`msys2` might give you a short introduction on these topics in the scope of Building OpenPASS itself.

.. _ref_prerequisites:

Referencing Prerequisites
-------------------------

With the migration from qmake to cmake, all prerequisite are independent of each other and simply can be refereced by using the right :ref:`CMAKE_PREFIX_PATH`.

.. warning:: 

   The following information is **DEPRECATED** and only needed if a ``qmake`` build shall be invoked.

Historically, |op_oss| uses a **single entry-point** for libraries and headers, so all prerequisites must to be located within a common folder structure.
If ``qmake`` needs to be invoked, and more than one prerequisite is customized, **a manual step** is necessary to establish this structure.

Example:

#. Build prerequisite_1
#. Build prerequisite_2
#. Copy libraries of both prerequisites into e.g. ``C:\OpenPASS\thirdParty\lib``
#. Copy common headers into e.g. ``C:\OpenPASS\thirdParty\include``

#. Reference the entry points by adding the following arguments to the qmake command

   .. code-block:: batch

      EXTRA_INCLUDE_PATH=c:\OpenPASS\thirdParty\include
      EXTRA_LIB_PATH=c:\OpenPASS\thirdParty\lib

   .. warning::
      
      Make sure that there is no linebreak between the two arguments.
      Qmake cannot handle this, but does not complain.

.. _building_osi:

Building OSI
------------

As teased in :ref:`Prerequisites`, the core component ``World_OSI`` uses :term:`OSI` as backend storage.
OSI itself uses ``protobuf`` to describe data structures in a platform independent way by means of `*.proto` files.
When building OSI, these files are converted into C++ headers and sources, using the protobuf compiler ``protoc``.
Finally, the sources are then compiled into a library.
|Op| finally uses the library and the generated headers to interface the library.

#. Download release 3.2.0 from https://github.com/OpenSimulationInterface/open-simulation-interface 

#. Extract to e.g. ``C:\OpenPASS\thirdParty\sources\open-simulationinterface``

#. Optional: Enable Arenas
   
   For better performance, |op| supports protobuf Arenas allocation (https://developers.google.com/protocol-buffers/docs/reference/arenas).
   To use this feature, OSI and |op| needs to be compiled with Arenas support.
   See :ref:`cmake_protobuf_arenas` how this feature is enabled in |op|.

   For OSI, ``option cc_enable_arenas = true;`` needs to be added **manually** to all OSI proto files before compilation.
   If everything goes well, ``protoc`` will generate all the magic necessary, otherwise issue an error during compilation.

#. Open |mingw_shell| and navigate to extracted folder

#. Create build directory

   .. code-block:: bash

      mkdir build
      cd build

#. Run Cmake

   .. code-block:: bash

      cmake -G “MinGW Makefiles” \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty \
            -DProtobuf_INCLUDE_DIR=C:/OpenPASS/thirdParty/include \
            -DProtobuf_PROTOC_EXECUTABLE=C:/OpenPASS/thirdParty/bin/protoc.exe \
            -DProtobuf_LIBRARIES=C:/OpenPASS/thirdParty/lib  \
            ..

   .. note:: 
   
      Here protobuf is used in as customized prerequisite (see :ref:`building_protobuf`).
      Adjust paths as needed, if already installed as system package.

#. Add linker flags for protobuf

   Unfortunantly OSI does not allow for to hook in a custom protobuf library.
   To force compilation against a custom library, edit ``CMakeFiles\open_simulation_interface.dir\linklibs.rsp``
   and add ``-LC:/OpenPASS/thirdParty/lib -lprotobuf`` to the end of the line.
   
   .. admonition:: A little bit hacky...
   
      If anybody knows how to avoid this step, please let us know.

#. Compile
   
   .. code-block:: bash

      mingw32-make -j3

#. Install

   .. code-block:: bash

      mingw32-make install

#. Deprecated: Establish unified layout for prerequisites (c.f. :ref:`ref_prerequisites`)

   .. code-block:: bash

      cd /C/OpenPASS/thirdParty
      cp lib/osi3/libopen_simulation_interface.dll lib
   
.. admonition:: Documentation
   
   The OSI class documentation is part of the source code and can be compiled using Doxygen.
   Instructions are located in the OSI ``Readme.md``. A pre-compiled version is located `here <https://opensimulationinterface.github.io/open-simulation-interface/index.html>`_. 
   
   So far, the documentation does not include the extensions from the openpass-trafficAgents branch.

.. _building_protobuf:

Building Protobuf
-----------------

.. note::
   
   Protobuf is already available as package for MSYS2 in different versions (see :ref:`msys2_packages`).

If a custom build is necessary, adjust this guide to your needs.
Exemplarily, it gives instructions, how to compile version 3.11.4. and hook it into the |op| build.

#. Download release 3.11.4 from https://github.com/protocolbuffers/protobuf/releases

#. Extract to e.g. ``C:\OpenPASS\thirdParty\sources\protobuf-cpp-3.11.4``

#. Open |mingw_shell| and navigate to extracted folder

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
            -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty \
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

#. Deprecated: Establish unified layout for prerequisites (c.f. :ref:`ref_prerequisites`)

   .. code-block:: bash
   
       cd /C/OpenPASS/thirdParty
       cp bin/libprotobuf.dll lib
  
.. note::

   Please refer to :ref:`runmingwexe` to see how to run the protobuf compiler ``protoc.exe`` outside of the |mingw_shell|.

.. _building_fmil:

Building FMIL
-------------

#. Download release 2.0.3 from https://github.com/modelon-community/fmi-library

#. Extract to e.g. ``C:\OpenPASS\thirdParty\sources\fmi-library``

#. Open |mingw_shell| and navigate to extracted folder

#. Create build directory

   .. code-block:: bash

      mkdir build
      cd build

#. Run Cmake

   .. code-block:: bash

      cmake -G "MinGW Makefiles" \
            -DFMILIB_INSTALL_PREFIX=C:/OpenPASS/thirdParty \
            -DCMAKE_BUILD_TYPE=Release \
            -DFMILIB_BUILD_STATIC_LIB=OFF \
            -DFMILIB_BUILD_SHARED_LIB=ON  \
            ..

#. Apply Patch (Linux only)
   
   As FMIL and the internally used `FMU Compliance Checker <https://github.com/modelica-tools/FMUComplianceChecker>`_ has issues with loading and private entry points under Linux, the following patch needs to be applied: :download:`Linux Patch </_static/resources/fmil/fmil203.patch>`

#. Compile

   .. code-block:: bash

      mingw32-make -j3
  
#. Install

   .. code-block:: bash
  
      mingw32-make install

#. Deprecated: Establish unified layout for prerequisites (c.f. :ref:`ref_prerequisites`)
    
   .. code-block:: bash
   
       cd /C/OpenPASS/thirdParty/include
       mkdir FMILibrary
       mv -r FMI FMILibrary
       mv -r FMI2 FMILibrary
       mv -r JM FMILibrary
       mv fmilib.h FMILibrary
       mv fmilib_config.h FMILibrary