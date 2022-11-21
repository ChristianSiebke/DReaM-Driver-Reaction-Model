<!---************************************************************
Copyright (c) 2021 in-tech GmbH
Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)

This program and the accompanying materials are made
available under the terms of the Eclipse Public License 2.0
which is available at https://www.eclipse.org/legal/epl-2.0/

SPDX-License-Identifier: EPL-2.0
************************************************************-->

# openPASS Documentation

Build this documentation with Sphinx, a python based documentation generator based on [reStructuredText](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html) as primary import format.

Sphinx
======

## Resources

- [reStructuredText Primer](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html)
- [A "How to" Guide for Sphinx + ReadTheDocs](https://source-rtd-tutorial.readthedocs.io/en/latest/index.html)
- [Sphinx Documentation](https://www.sphinx-doc.org)
- [OSI Sphinx Config](https://github.com/OpenSimulationInterface/osi-documentation/blob/master/conf.py)


## Building the documentation

```
# install python, pip, spellchecker, ...
sudo apt install doxygen python3 python3-pip dvipng

# install sphinx and its extensions
pip3 install sphinx sphinx-rtd-theme sphinx-tabs breathe exhale sphinxcontrib-spelling

# build doc (only)
mkdir build
cd build
cmake -DWITH_SIMCORE=OFF -DWITH_TESTS=OFF -DWITH_DOC=ON ..
make doc
```
