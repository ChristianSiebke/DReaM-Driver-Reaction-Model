..
  ************************************************************
  Copyright (c) 2021 in-tech GmbH

  This program and the accompanying materials are made
  available under the terms of the Eclipse Public License 2.0
  which is available at https://www.eclipse.org/legal/epl-2.0/

  SPDX-License-Identifier: EPL-2.0
  ************************************************************

.. _sphinx:

Sphinx
======

What is Sphinx
--------------

Sphinx is a python based documentation generator, using `reStructuredText <https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html>`_ as primary import format.

Who uses Sphinx
---------------

- Everybody who publishes on `ReadTheDocs <https://readthedocs.org/>`_
- `OSI <https://github.com/OpenSimulationInterface/osi-documentation>`_

Why not markdown
----------------

- Markdown was not invented to write documentation
- Right now the doxygen implementation is not pure markdown, which makes it a bit nasty to use
- Markdown does not support true references (and reference checking), figures (and captions), toc, glossary, versioning, ...
- Markdown is also supported by sphinx, but does not integrate with the cool and fancy features.

Resources
---------

- `reStructuredText Primer <https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html>`_
- `A "How to" Guide for Sphinx + ReadTheDocs <https://source-rtd-tutorial.readthedocs.io/en/latest/index.html>`_
- `Sphinx Documentation <https://www.sphinx-doc.org>`_
- `OSI Sphinx Config <https://github.com/OpenSimulationInterface/osi-documentation/blob/master/conf.py>`_

Building this Documentation (Linux/Debian)
------------------------------------------

.. code-block:: bash

   # install python, pip, sphinx, spellchecker, ...
   sudo apt install doxygen python3 python3-pip python3-sphinx libenchant-2-2 dvipng
   
   # install sphinx extensions
   pip3 install sphinx sphinx-rtd-theme sphinx-tabs breathe exhale sphinxcontrib-spelling

   # build doc (only)
   mkdir build
   cd build
   cmake -DWITH_SIMCORE=OFF -DWITH_TESTS=OFF -DWITH_DOC=ON ..
   make doc
