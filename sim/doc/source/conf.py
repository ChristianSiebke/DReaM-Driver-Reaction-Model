#*************************************************************
# Copyright (c) 2021 in-tech GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
#*************************************************************

# Configuration file for the Sphinx documentation builder. See also:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os 
import sys
import datetime
import sphinx_rtd_theme
import sphinxcontrib.spelling
from textwrap import dedent

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.

# -- Project information -----------------------------------------------------
project = 'openPASS'
copyright = f'{datetime.datetime.now().year} OpenPASS Working Group'
author = 'in-tech GmbH'

generate_api_doc = '-DWITH_API_DOC=ON' in sys.argv
if generate_api_doc:
    sys.argv.remove('-DWITH_API_DOC=ON')

# -- Version is generated via cmake
version_file = 'version.txt'
if os.path.exists(version_file):
    with open(version_file) as vf:
        version = vf.read().strip()
        release = version

# -- General configuration ---------------------------------------------------
def setup(app):
   app.add_css_file('css/custom.css')
   
extensions = []
extensions.append('sphinxcontrib.spelling')
extensions.append("sphinx_rtd_theme")
extensions.append('sphinx.ext.todo')
extensions.append('sphinx_tabs.tabs')
extensions.append('sphinx.ext.imgmath')

templates_path = ['_templates']

exclude_patterns = []

todo_include_todos = True

pdf_stylesheets = ['sphinx', 'kerning', 'a4']
pdf_style_path = ['.', '_styles']
pdf_fit_mode = "shrink" # literal blocks wider than frame
pdf_language = "en_US"
pdf_page_template = 'cutePage'

# -- Options developer documentation --------------------------------------
if generate_api_doc:
    extensions.append('breathe')
    extensions.append('exhale')

    breathe_projects = {"openpass":"doxyoutput/xml"}
    breathe_default_project = "openpass"

    # -- Setup the exhale extension ----------------------------------------------
    exhale_args = {
        # These arguments are required
        "containmentFolder":     "./api",
        "rootFileName":          "index.rst",
        "rootFileTitle":         "Source Code Documentation",
        "doxygenStripFromPath":  "@OP_REL_SIM@",
        # Suggested optional arguments
        "createTreeView":        True,
        # TIP: if using the sphinx-bootstrap-theme, you need
        # "treeViewIsBootstrap": True,
        "exhaleExecutesDoxygen": True,
        "exhaleDoxygenStdin":    dedent('''   
            INPUT                 = @OP_REL_SIM@/src
            WARN_AS_ERROR         = NO
            PREDEFINED           += DOXYGEN_SHOULD_SKIP_THIS
            GENERATE_HTML         = NO
            RECURSIVE             = YES
            FULL_PATH_NAMES       = YES
            QUIET                 = YES
            FILE_PATTERNS         = *.cpp *.h
            ENABLE_PREPROCESSING  = YES
            MACRO_EXPANSION       = YES
            SKIP_FUNCTION_MACROS  = NO
            EXPAND_ONLY_PREDEF    = NO
        ''')
    }
    
    # make TOC available
    with open('api.rst.template', 'r') as api_toc_template:
        with open('api.rst', 'w') as api_toc:
            api_toc.write(api_toc_template.read())
else:
    # wipe TOC for developer doc by creating an empty file
    open('api.rst', 'w').close()

# -- Options for HTML output -------------------------------------------------

html_static_path = ['_static']
html_theme = 'sphinx_rtd_theme'
html_title = 'OpenPASS Documentation'
html_short_title = 'OpenPASS|Doc'
html_favicon = '_static/openPASS.ico'
html_logo = '_static/openPASS.png'

# -- Define global replacements ----------------------------------------------
# See https://documentation.help/Sphinx/config.html
rst_epilog = """

.. |op| replace:: **openPASS**
.. |Op| replace:: **OpenPASS**
.. |opwg| replace:: **openPASS** Working Group
.. |op_oss| replace:: **openPASS** (Open Source)
.. |Op_oss| replace:: **OpenPASS** (Open Source)
.. |mingw_shell| replace:: ``MinGW 64-bit`` shell

"""
