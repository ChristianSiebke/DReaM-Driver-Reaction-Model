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

Installing OpenPASS
===================

This section describes how compile and run |op|. Please make sure that all prerequisites have been properly installed according to section :ref:`Prerequisites`.
If you have strictly followed the instructions, the installed source packages should be located on your machine under 
``C:\OpenPASS\thirdParty`` for **Windows** and ``~/OpenPASS/thirdParty`` for **Linux**. If there is a path deviation, 
the following commands must be adjusted.

To keep the installation process of |op| as simple as possible, we again recommend a specific folder structure.
If you have strictly followed and will follow the recommended paths of this guide, no command modifications are necessary.

.. tabs::

   .. tab:: Notes for Windows

      - Recommended checkout path of |op| repository: ``C:\simopenpass``
      - Recommended dependendy directory within the |op| repository: ``C:\simopenpass\deps\thirdParty``
      - Recommended installation directory of |op|: ``C:\OpenPASS\bin\core``

   .. tab:: Notes for Linux

      - Recommended checkout path of |op| repository: ``~/simopenpass``
      - Recommended dependendy directory within the |op| repository: ``~/simopenpass/deps/thirdParty``
      - Recommended installation directory of |op|: ``/usr/local/OpenPASS/bin/core``
 
The above directory structure will be created by following the instructions of this guide. 

#. Clone |Op| repository

   As described above, the checkout path of the repository is assumed to be the default ``simopenpass``.

   .. tabs::

      .. tab:: Windows

         Start |mingw_shell|
         
         .. code-block:: 

            cd /C/
            git clone https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git

         .. note::

            As stated in :ref:`Building_under_windows`, the windows programming tools suffer from a `path length restriction`.
            This error manifests as strange **file not found** compile errors.
            It is therefore recommended to use a short path for source code checkout, e.g. a drive letter.
            This can also be done by the windows command `subst <https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/subst>`_.

      .. tab:: Linux

         Start ``Bash`` shell

         .. code-block:: 
         
            cd ~
            git clone https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git

#. Navigate into repository and checkout master branch

   .. code-block:: 

      cd simopenpass
      git checkout master

#. Create directory structure

   .. code-block:: 

      mkdir -p deps/thirdParty

#. Navigate into directory where installed prerequisites are located

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cd /C/OpenPASS/thirdParty

      .. tab:: Linux

         .. code-block:: 
      
               cd ~/OpenPASS/thirdParty

#. Copy prerequisites into repository (c.f. :ref:`Prerequisites`)

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cp -r osi /C/simopenpass/deps/thirdParty
            cp -r FMILibrary /C/simopenpass/deps/thirdParty

      .. tab:: Linux

         .. code-block:: 
      
            cp -r osi ~/simopenpass/deps/thirdParty
            cp -r FMILibrary ~/simopenpass/deps/thirdParty

   .. _ref_prerequisites:

   Once prerequisites are in place, all third party dependencies within ``simopenpass`` can be resolved by ``CMake``. 
   For ``CMake`` builds, each installed prerequisite, which is located in its own subdirectory under 

   - ``C:\simopenpass\deps\thirdParty`` for Windows and
   - ``~/simopenpass/deps/thirdParty`` for Linux,
  
   can be referenced by its path. 
   The reference is made via the :ref:`CMAKE_PREFIX_PATH` environmental variable, which holds a list of directories specifying the installed prerequisite. 
   This will be explained in more detail in the following.

#. Navigate back into repository

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cd /C/simopenpass

      .. tab:: Linux

         .. code-block:: 
      
            cd ~/simopenpass

#. Create build directory and navigate into it

   .. code-block:: 
      
      mkdir build
      cd build

#. Prepare build

   |Op| links against shared libraries, which are located in the paths specified by ``CMAKE_PREFIX_PATH``. 
   To be able to install |Op| with resolved dependencies, all libraries found under the paths have to be copied right next to the executable during the installation step. 
   This is done by setting ``INSTALL_EXTRA_RUNTIME_DEPS=ON``. If you have followed the instructions strictly, no changes are necessary. 

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cmake -G "MSYS Makefiles" \
            -D CMAKE_PREFIX_PATH="C:\msys64\mingw64\bin;C:\simopenpass\deps\thirdParty\FMILibrary;C:\simopenpass\deps\thirdParty\osi;" \
            -D CMAKE_INSTALL_PREFIX=/C/OpenPASS/bin/core \
            -D CMAKE_BUILD_TYPE=Release \
            -D USE_CCACHE=ON \
            -D WITH_DEBUG_POSTFIX=OFF \
            -D OPENPASS_ADJUST_OUTPUT=OFF \
            -D INSTALL_EXTRA_RUNTIME_DEPS=ON \
            -D CMAKE_C_COMPILER=gcc \
            -D CMAKE_CXX_COMPILER=g++ \
            ..

         .. note:: Even though it is recommended, you do not have to copy :term:`MinGW` libraries next to the executable. Providing the libraries can also be done in the following ways:

                   - either |op| gets **only** executed exclusively from the |mingw_shell|, then all necessary :term:`MinGW` libraries get linked automatically by the shell
                   - or one can add ``C:\msys64\mingw64\bin`` permanently to the *Windows Environment Variable* ``Path``
                   - or temporarily set ``Path`` prior to the execution, e.g. in a wrapper:

                     .. code-block:: batch
                           
                        # your_program.cmd
                        Path=C:\msys64\mingw64\bin;%Path% # set Path
                        your_program.exe                  # execute

      .. tab:: Linux

         .. code-block:: 

            cmake -D CMAKE_PREFIX_PATH="~/simopenpass/deps/thirdParty/FMILibrary;~/simopenpass/deps/thirdParty/osi" \
            -D CMAKE_INSTALL_PREFIX=/usr/local/OpenPASS/bin/core \
            -D CMAKE_BUILD_TYPE=Release \
            -D USE_CCACHE=ON \
            -D WITH_DEBUG_POSTFIX=OFF \
            -D OPENPASS_ADJUST_OUTPUT=OFF \
            -D INSTALL_EXTRA_RUNTIME_DEPS=ON \
            -D CMAKE_C_COMPILER=gcc-9 \
            -D CMAKE_CXX_COMPILER=g++-9 \
            ..
   
   .. note:: If you need to adjust paths and options based on your system and needs and don't forget to escape the semicolon ``;`` (see :ref:`Cmake_prefix_path`). 
             For a build that goes beyond the default settings, see :ref:`Cmake` for more available variables and options that can be set.
             

#. Optional: Build and execute unit tests

   Starting from ``simopenpass/build``:

   .. code-block:: 

      make test ARGS="--output-on-failure -j3"

   .. note::

      ARGS is optional, but recommended.
      Adjust parallel build flag ``-j3`` based on your system.

#. Build simulation core

   Starting from ``simopenpass/build``:

   .. code-block:: 

      make -j3 install

   .. note:: Again, adjust parallel build flag ``-j3`` based on your system.

#. Fix installation (we're working on that...)

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cp /C/OpenPASS/bin/core/bin/* /C/OpenPASS/bin/core

      .. tab:: Linux

         .. code-block:: 

            cp /usr/local/OpenPASS/bin/core/bin/* /usr/local/OpenPASS/bin/core