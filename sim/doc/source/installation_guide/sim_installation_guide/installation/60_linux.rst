..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _building_under_linux:

Building under Linux
====================

Before building |op|, install the according :ref:`prerequisites`.
OpenPass is developed under Debian 64-Bit, which means that developing under a recent Ubuntu distribution should also work.
Debian uses ``apt`` (or ``apt-get``) as package managing system, and most prerequisites should be installable via ``app install package``.

This snippet shows an example for building |op_oss|.
Refer to :ref:`cmake` for details on the arguments.

.. code-block:: bash
   :emphasize-lines: 9

   cmake 
     -D CMAKE_BUILD_TYPE=Debug \
     -D OPENPASS_ADJUST_OUTPUT=OFF \
     -D USE_CCACHE=ON \
     -D WITH_DEBUG_POSTFIX=OFF \
     -D WITH_GUI=OFF \
     -D WITH_PROTOBUF_ARENA=ON \
     -D CMAKE_INSTALL_PREFIX=/OpenPASS/bin/core \
     -D CMAKE_PREFIX_PATH=PATH_FMIL\;PATH_BOOST\;PATH_OSI\;PATH_PROTOBUF\;PATH_GTEST \
     ..
     
     make -j3
     make install

.. note::
   
   Don't forget to escape the semicolon ``;`` (see :ref:`CMAKE_PREFIX_PATH`)
