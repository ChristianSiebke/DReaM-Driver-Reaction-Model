..
  *******************************************************************************
  Copyright (c) 2021 ITK Engineering GmbH

  This program and the accompanying materials are made available under the
  terms of the Eclipse Public License 2.0 which is available at
  http://www.eclipse.org/legal/epl-2.0.

  SPDX-License-Identifier: EPL-2.0
  *******************************************************************************

.. _building_with_conan:

Building with Conan
===================

.. image:: https://conan.io/img/logo-conan.svg

General
-------

Conan is an open source, decentralized and multi-platform package manager to create and share native binaries.

To learn more about Conan itself you can visit `Conan <https://conan.io/>`_.

Conan is used in the |op| project to create and manage the binaries of the used ThirdParty libraries.

Additional Prerequisites
--------------------------

To be able to work with Conan it´s necessary to add additional packages.

.. tabs::
   
   .. tab:: Windows (MSYS2)

      .. code-block:: bash

         pip install conan

   .. tab:: Linux (Debian Bullseye)

      .. code-block:: bash

         apt install conan


Export Recipes
--------------

.. note::

   A *recipe* is python file, which is used by conan to build and create a package. 
   In this file there are several hooks available, which can be used for several tasks.
   E.g. Retrieving the source files, building from source, configuring dependencies, packaging, etc.
   Visit `Conan <https://conan.io/>`_. for detailed information about *recipes*.


To make Conan aware and to use the custom recipes for the ThirdParty libraries, these recipes have to be exported to the *local cache* of Conan.
This can be done in different ways.

Conan Export
^^^^^^^^^^^^

.. code-block:: bash

   cd <path to conanfile.py of recipe>
   conan export . <optional user>/<optional channel>

The optional user and channel flags can be used to export a recipe while it´s still in development.
In that case there could be several different versions and their corresponding binaries of the same library available at the same time.
The *local cache* could then show the following packages.
E.g.

.. code-block:: bash

   packagename/3.2.0
   packagename/3.2.0@openpassWG/stable 
   packagename/3.3.0@myUserName/testing

.. note::

   At least this step has to be done in prior before any ThirdParty library can be installed by Conan.
   Every upcoming step is optional, but can be useful while developing or debugging.
   Also with these steps it´s possible to install ThirdParty libraries separately.

Conan Install
^^^^^^^^^^^^^

Conan is now able to differentiate different version and the user can now install exactly the version he wishes to.

.. code-block:: bash

   conan install <package_name>

This command has a look into the *local cache* for the recipe and the binaries of the package and installs them into the *local cache*.
If no binaries where found an error is thrown. 

.. note::

   To make conan also build the package the command can be appended with the following argument: ``--build=missing``.

Conan Create
^^^^^^^^^^^^

Both of the previous commands can be combined with the ``create``` command. What it basically does is calling ``export`` and ``install`` after another.

.. code-block:: bash

   cd <path to conanfile.py of recipe>
   conan create . <optional user>/<optional channel>

At this step the binaries of this package are now available in the *local cache*. 

.. note::

   There are several additional commands, arguments and fields for these commands available. 
   Please see `Conan Docs <https://docs.conan.io/en/latest/>`_ for additional information.

Build |op|
----------

If you followed the previous steps and export all dependencies and |op| recipes to Conans *local cache*, then you can go on with the next steps.
If not then at least the ``export`` should be done. 
Else Conan is not able to know how to build the dependencies and |op|.

|op| can be build using a ``conanfile.txt``. 
It holds every Information which is needed to build and install |op|.

The following file shows an example which can be used. 
This example is also available in the ``conan`` folder in the ``simopenpass`` repository.

.. code-block:: 

   [requires]
      openpass/0.8

   [generators]
      cmake

   [imports]
      . , * -> C:/openpass @

The ``requires`` field describes what package with what version shall be installed. 
It´s also possible to add more packages here or change the version.

The ``generator`` field is used to add different generators to the process. 
In this example the ``cmake`` generator would generate cmake files, which describe where to find the |op| in the *local cache*. 
There are also other possibilities like a ``cmake_find_package`` generator, which generates *cmake* *FindPackage* files. 

At this point |op| only exists in the *local cache* of Conan. 
With the ``imports`` field it´s possible to specify where and how the binaries shall be copied from the *local cache* to the *user space*.
The example follows the following pattern:

   <source>, <pattern> -> <destination>

.. note::

   It is possible to add several lines for more specific copying operations. 
   Also there are more field available than this simple example shows.
   See `Conan Docs <https://docs.conan.io/en/latest/>`_ for additional information.

Conan can simply install |op| with the following command. 
If the dependencies or |op| were not build in any previous step, the binaries will not be available in the *local cache* of Conan. 
Conan can build them by adding ``--build=missing`` to the following command.

.. code-block:: bash

   conan install <path to conanfile.txt>


Build only ThirdParties
-----------------------

The ThirdParty libraries can be build and installed the same way.

.. code-block:: 

   [requires]
      OSI/3.2.0
      FMILibrary/2.0.3

   [generators]
      cmake_find_package
      cmake_paths

   [imports]
      . , * -> C:/openpass/ThirdParty @
