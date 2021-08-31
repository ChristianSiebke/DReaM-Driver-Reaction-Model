..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH
                2020 BMW AG

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

[Deprecated]: qmake Build
~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning:: 

   The following information is **DEPRECATED** and only needed if a ``qmake`` build shall be invoked.

Historically, |op_oss| uses a **single entry-point** for libraries and headers, so all prerequisites had to be located within a common folder structure:

::
   
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