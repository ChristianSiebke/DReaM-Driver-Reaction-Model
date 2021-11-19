..
  *******************************************************************************
  Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
                2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************
  
.. _building_protobuf:

Custom Protobuf Build
*********************

Build and Install Protobuf
==========================

As Protobuf can be installed easily as binary package in different versions (see :ref:`Binary_packages`) via appropiate package managing systems, there is no need to build protobuf from source.
However, if a custom build is necessary, this section gives instructions, how to compile version 3.11.4. and hook it into the |op| build.
Adjust this guide to your needs.

#. Download release 3.11.4 from https://github.com/protocolbuffers/protobuf/releases

#. Extract

   - for Windows to ``C:\OpenPASS\thirdParty\sources\protobuf-cpp-3.11.4``

   - for Linux to ``~/OpenPASS/thirdParty/sources/protobuf-cpp-3.11.4``

#. Navigate to the extracted folder

   .. tabs::

      .. tab:: Windows

         Start |mingw_shell|

         .. code-block:: 

            cd /C/OpenPASS/thirdParty/sources/protobuf-cpp-3.11.4

      .. tab:: Linux

         Start ``Bash`` shell

         .. code-block:: 
      
            cd ~/OpenPASS/thirdParty/sources/protobuf-cpp-3.11.4

#. Create build directory

   .. code-block:: 
      
      cd cmake
      mkdir build
      cd build

#. Run Cmake

   .. tabs::

      .. tab:: Windows

         .. code-block:: 
            :emphasize-lines: 6

            cmake -G "MSYS Makefiles" \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty/protobuf \
                  -Dprotobuf_BUILD_SHARED_LIBS=ON \
                  -Dprotobuf_BUILD_TESTS=OFF \
                  ..

      .. tab:: Linux

         .. code-block:: 
            :emphasize-lines: 5

            cmake -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX=~/OpenPASS/thirdParty/protobuf \
                  -Dprotobuf_BUILD_SHARED_LIBS=ON \
                  -Dprotobuf_BUILD_TESTS=OFF \
                  ..

   .. note::

      Tests are disabled due to compiler warnings treated as errors (may vary with compiler version).

#. Compile

   .. code-block:: 

      make -j3

#. Install

   .. code-block:: 

      make install


Hook Protobuf into OSI
======================

If you want to build and install OSI with a custom build of Protobuf, most of the 
steps can be followed as described in instruction :ref:`Building_osi`. However, one 
step must be added.

For all required steps *before* the following command, see :ref:`Building_osi`.
  
#. Additional ``CMake`` variables have to be set.

   .. tabs::

      .. tab:: Windows

         .. code-block:: 
            :emphasize-lines: 7

            cmake -G “MSYS Makefiles” \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX=C:/OpenPASS/thirdParty/osi \
                  -DPROTOBUF_INCLUDE_DIR=C:/OpenPASS/thirdParty/protobuf/include \
                  -DPROTOBUF_PROTOC_EXECUTABLE=C:/OpenPASS/thirdParty/protobuf/bin/protoc.exe \
                  -DPROTOBUF_LIBRARY=C:/OpenPASS/thirdParty/protobuf/lib/libprotobuf.a  \
                  ..

      .. tab:: Linux

         .. code-block:: 
            :emphasize-lines: 6

            cmake -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX=~/OpenPASS/thirdParty/osi \
                  -DPROTOBUF_INCLUDE_DIR=~/OpenPASS/thirdParty/protobuf/include \
                  -DPROTOBUF_PROTOC_EXECUTABLE=~/OpenPASS/thirdParty/protobuf/bin/protoc \
                  -DPROTOBUF_LIBRARY=~/OpenPASS/thirdParty/protobuf/lib/libprotobuf.a  \
                  ..

For all required steps *after* this stage, see :ref:`Building_osi`.