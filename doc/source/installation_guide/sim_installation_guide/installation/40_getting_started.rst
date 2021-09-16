..
  ************************************************************
  Copyright (c) 2021 BMW AG

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _getting_started:

Getting Started
===============

Using the recommended repository path and install path, you can run an example simulation by doing the following steps:

#. Navigate into |op| install directory

   .. tabs::

      .. tab:: Windows

         Start |mingw_shell|

         .. code-block:: 

            cd /C/OpenPASS/bin/core

      .. tab:: Linux

         Start ``Bash`` shell

         .. code-block:: 

            cd /usr/local/OpenPASS/bin/core


#. Create directory for default configs

   .. code-block:: 

      mkdir configs


#. Copy default configs from repository into install directory

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            cp /C/simopenpass/sim/contrib/examples/Common/* /C/OpenPASS/bin/core/configs

      .. tab:: Linux

         .. code-block:: 

            cp ~/simopenpass/sim/contrib/examples/Common/* /usr/local/OpenPASS/bin/core/configs

#. Start executable

   .. tabs::

      .. tab:: Windows

         .. code-block:: 

            ./OpenPassSlave.exe

      .. tab:: Linux

         .. code-block:: 

            ./OpenPassSlave

See :ref:`sim_user_guide` for further explanation. 


  
