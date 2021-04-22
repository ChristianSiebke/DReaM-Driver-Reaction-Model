..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH
                2021 BMW AG

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _download_and_install_openpass:

Build and Install OpenPASS
=============================

This section describes how compile and run |op|.

Please note:

- ``C:\simopenpass`` refers to the standard checkout path when cloning the |op| repository
- ``C:\simopenpass\deps\thirdParty`` refers to a directory within the |op| repository  
- ``C:\OpenPASS\thirdParty`` refers to a temporary directory used to built the prerequisites from source, **not** the |op| repository
- ``C:\OpenPASS\bin`` refers to the installation directory
 
.. note::

   For **Windows**, an up-to-date MinGW 64-bit environment is assumed, for **Linux**, Debian Bullseye or Ubuntu 20.10 is recommended.

#. Install dependencies. See :ref:`Prerequisites` for binary packages and :ref:`Building prerequisites` for source packages. 
   
   For brevity, the following description assumes that installed source packages are located on your machine at ``C:\OpenPASS\thirdParty``. 
   if you have strictly followed the recommended paths of this guide, this is automatically fulfilled. 
   As will be stated soon, the prerequisites will be copied from ``C:\OpenPASS\thirdParty`` into the |op| repository.

#. Clone |Op| repository

   For brevity, the following description assumes that the checkout path of the repository is the default ``simopenpass``.

   .. tabs::

      .. tab:: Windows

         As stated in :ref:`Building_under_windows`, the windows programming tools suffer from a `path length restriction`.
         It is therefore recommended to use a short path for source code checkout, e.g. a drive letter.

         .. code-block:: bash

            cd /C/
            git clone https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git

      .. tab:: Linux

         .. code-block:: bash
         
            git clone https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git

#. Navigate into repository and checkout master branch

   .. code-block:: bash

      cd simopenpass
      git checkout master

#. Create directory structure

   .. code-block:: bash

      mkdir -p deps/thirdParty

#. Navigate into directory where installed prerequisites are located

   .. code-block:: bash

      cd /C/OpenPASS/thirdParty

#. Copy prerequisites into repository (c.f. :ref:`ref_prerequisites`)

   .. code-block:: bash

      cp -r osi /C/simopenpass/deps/thirdParty
      cp -r FMILibrary /C/simopenpass/deps/thirdParty

   .. note::

      If custom protobuf build is used, add 

      .. code-block:: bash

         cp -r protobuf /C/simopenpass/deps/thirdParty

   .. _ref_prerequisites:

   Once prerequisites are in place, all third party dependencies within ``simopenpass`` can be resolved by ``CMake``. 
   For ``CMake`` builds, each installed prerequisite, which is located in its own subdirectory under ``simopenpass\deps\thirdParty``, can be referenced by its path. 
   The reference is made via the :ref:`CMAKE_PREFIX_PATH` environmental variable, which holds a list of directories specifying the installed prerequisite. 
   This will be explained in more detail in the following.

#. Navigate back into repository

   .. code-block:: bash

      cd /C/simopenpass

#. Create build directory and navigate into it

   ..code-block:: bash
      
      mkdir build
      cd build

#. Prepare build

   Starting from ``simopenpass/build``

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            cmake -G "MinGW Makefiles" \
            -D CMAKE_PREFIX_PATH="C:\msys64\mingw64\bin;{CMAKE_CURRENT_SOURCE_DIR}\..\deps\thirdParty\FMILibrary;{CMAKE_CURRENT_SOURCE_DIR}\..\deps\thirdParty\osi;" \
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

            cmake -D CMAKE_PREFIX_PATH=/opt/qt5.12.3/5.12.3/gcc_64\;\
                     ../deps/thirdParty/FMILibrary\;\
                     ../deps/thirdParty/boost\;\
                     ../deps/thirdParty/osi\;\
                     ../deps/thirdParty/minizip\;\
                     ../deps/thirdParty/protobuf\;\
                     ../deps/thirdParty/googletest \
                  -D CMAKE_INSTALL_PREFIX=/OpenPASS/bin/core \
                  -D CMAKE_BUILD_TYPE=Release \
                  -D CMAKE_WITH_DEBUG_POSTIX=OFF \
                  -D OPENPASS_ADJUST_OUTPUT=OFF \
                  -D USE_CCACHE=ON \
                  -D CMAKE_C_COMPILER=gcc-10 \
                  -D CMAKE_CXX_COMPILER=g++-10 \
                  ..

   .. note:: Adjust paths and options based on your system and needs and don't forget to escape the semicolon ``;`` (see :ref:`CMAKE_PREFIX_PATH`)

#. Optional: Build and execute unit tests

   Starting from ``simopenpass/build``:

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            mingw32-make test ARGS="--output-on-failure -j3"

      .. tab:: Linux

         .. code-block:: bash

            make test ARGS="--output-on-failure -j3"

   .. note::

      ARGS is optional, but recommended.
      Adjust parallel build flag ``-j3`` based on your system.

#. Build simulation core

   Starting from ``simopenpass/build``:

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            mingw32-make -j3 install

         .. note::
   
            The call to mingw32-make might be misleading, but actually calls gcc/g++ 64-bit.

      .. tab:: Linux

         .. code-block:: bash
   
            make -j3 install

   .. note:: Again, adjust parallel build flag ``-j3`` based on your system.

#. Fix installation (we're working on that...)

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            cp /C/OpenPASS/bin/core/bin/* /C/OpenPASS/bin/core

      .. tab:: Linux

         .. code-block:: bash

            cp ./OpenPASS/bin/core/bin/* ./OpenPASS/bin/core

#. Provide libraries

   .. tabs::

      .. tab:: Windows

         Firstly, osi and FMILibrary library have to be provided. This can be done by manual copying or using the |mingw_shell|:

         .. code-block:: bash

            cp /C/simopenpass/deps/thirdParty/{osi/lib/osi3/libopen_simulation_interface.dll,FMILibrary/lib/libfmilib_shared.dll} /C/OpenPASS/bin/core

         Secondly, files compiled within the |mingw_shell| depend on the following :term:`MinGW` libraries located under ``C:\msys64\mingw64\bin``

         - libboost_filesystem-mt.dll
         - libdouble-conversion.dll
         - libgcc_s_seh-1.dll
         - libicudt68.dll
         - libicuin68.dll
         - libicuuc68.dll
         - libpcre2-16-0.dll
         - libstdc++-6.dll
         - libwinpthread-1.dll
         - libzstd.dll
         - zlib1.dll

         Copy the required libraries right next to your executable either by manual copying or by using the |mingw_shell|:

         .. code-block:: batch

            cp /C/msys64/mingw64/bin/{libboost_filesystem-mt.dll,libdouble-conversion.dll,libgcc_s_seh-1.dll,libicudt68.dll,libicuin68.dll,libicuuc68.dll,libpcre2-16-0.dll,libstdc++-6.dll,libwinpthread-1.dll,libzstd.dll,zlib1.dll} /C/OpenPASS/bin/core

         .. warning::
         
            You might need to update the some libraries manually, when package are upgraded.

         .. note::

            You do not have to copy these libraries next to the executable. Providing the libraries can also be done in the following ways:

            - either |op| gets :term:`MinGW` libraries **only** executed exclusively from the |mingw_shell|. Then, all necessary libraries get linked automatically by the shell
            - or one can add ``C:\msys64\mingw64\bin`` permanently to the *Windows Environment Variable* ``Path``
            - or temporarily set ``Path`` prior to the execution, e.g. in a wrapper:

               .. code-block:: batch
                     
                  # your_program.cmd
                  Path=C:\msys64\mingw64\bin;%Path% # set Path
                  your_program.exe                  # execute

      .. tab:: Linux

         Under Linux, no additional libraries have to be provided.

#. Run simulation (example)

   Starting from ``simopenpass/build`` and using an example configuration:

   .. tabs::

      .. tab:: Windows

         .. code-block:: bash

            ./C/OpenPASS/bin/core/OpenPassSlave --config ../sim/contrib/examples/DefaultConfigurations

      .. tab:: Linux

         .. code-block:: bash

            ./OpenPASS/bin/core/OpenPassSlave --config ../sim/contrib/examples/DefaultConfigurations
