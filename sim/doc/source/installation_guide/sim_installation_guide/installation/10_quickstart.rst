..
  *******************************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

Quick Start
===========

This section describes how compile and run |op| as quickly as possible.

.. note:

   For **Windows**, an up-to-date MinGW 64-bit environment is assumed, for **Linux**, Debian Bullseye or Ubuntu 20.10 is recommended.

#. Install :ref:`Prerequisites`

   For brevity, the following description assumes that prerequisites are located at within the repository at ``deps/thirdParty``.

#. Get repository and submodules

   For brevity, the following description assumes that the checkout path is the default ``simopenpass``.

   .. code-block:: bash

      git clone https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git
      cd simopenpass

#. Prepare build

   Starting from ``simopenpass``

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            mkdir build
            cd build
            cmake -G "MinGW Makefiles"\
                  -D CMAKE_PREFIX_PATH="/mingw64/bin;\
                     ../deps/thirdParty/win64/FMILibrary;\
                     ../deps/thirdParty/win64/osi;"\
                  -D CMAKE_INSTALL_PREFIX=/C/OpenPASS/bin/core \
                  -D CMAKE_BUILD_TYPE=Release \
                  -D CMAKE_WITH_DEBUG_POSTIX=OFF \
                  -D OPENPASS_ADJUST_OUTPUT=OFF \
                  -D USE_CCACHE=ON \
                  -D CMAKE_C_COMPILER=gcc \
                  -D CMAKE_CXX_COMPILER=g++ \
                  ..

      .. tab:: Linux

         .. code-block:: bash

            mkdir build
            cd build
            cmake -D CMAKE_PREFIX_PATH=/opt/qt5.12.3/5.12.3/gcc_64\;\
                     ../deps/thirdParty/linux64/FMILibrary\;\
                     ../deps/thirdParty/linux64/boost\;\
                     ../deps/thirdParty/linux64/osi\;\
                     ../deps/thirdParty/linux64/protobuf\;\
                     ../deps/thirdParty/linux64/googletest \
                  -D CMAKE_INSTALL_PREFIX=/OpenPASS/bin/core \
                  -D CMAKE_BUILD_TYPE=Release \
                  -D CMAKE_WITH_DEBUG_POSTIX=OFF \
                  -D OPENPASS_ADJUST_OUTPUT=OFF \
                  -D USE_CCACHE=ON \
                  -D CMAKE_C_COMPILER=gcc-10 \
                  -D CMAKE_CXX_COMPILER=g++-10 \
                  ..

   .. note:: Adjust paths and options based on your system and needs.

#. Optional: Build and execute unit tests

   Starting from ``simopenpass/build``:

   .. code-block:: bash

      make test ARGS="--output-on-failure -j3"

   .. note::

      ARGS is optional, but recommended.
      Adjust parallel build flag ``-j3`` based on your system.

#. Build simulation core

   Starting from ``simopenpass/build``:

   .. code-block:: bash

      make -j3 install

   .. note:: Again, adjust parallel build flag ``-j3`` based on your system.

#. Fix installation (we're working on that...)

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            cp /C/OpenPASS/bin/core/bin /C/OpenPASS/bin/core

      .. tab:: Linux

         .. code-block:: bash

            cp ./OpenPASS/bin/core/bin ./OpenPASS/bin/core

#. Run simulation (example)

   Starting from ``simopenpass/build`` and using an example configuration:

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            ./C/OpenPASS/bin/core/OpenPassSlave --config ../sim/contrib/examples/AEB

      .. tab:: Linux

         .. code-block:: bash

            ./OpenPASS/bin/core/OpenPassSlave --config ../sim/contrib/examples/AEB