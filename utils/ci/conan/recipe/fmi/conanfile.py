################################################################################
# Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#               2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

################################################################################
# Install file for building FMILibrary with Conan
################################################################################

from conans import ConanFile, CMake, tools

class FmiConan(ConanFile):
    name = "FMILibrary"
    version = "2.0.3"
    license = "2-Clause BSD"
    author = "Michael Scharfenberg michael.scharfenberg@itk-engineering.de"
    url = "https://github.com/modelon-community"
    description = "The Functional Mock-up Interface (or FMI) defines a standardized interface to be used in computer simulations to develop complex cyber-physical systems"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    exports_sources = "fmi-library-2.0.3-fixes.patch"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self): 
        git = tools.Git(folder="fmi")
        git.clone("https://github.com/modelon-community/fmi-library.git", "2.0.3", "--recursive")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="fmi",
                        defs={"FMILIB_INSTALL_PREFIX":"./temp-deploy",
                              "FMILIB_BUILD_STATIC_LIB":"OFF",
                              "FMILIB_BUILD_SHARED_LIB":"ON"})
        
        tools.patch(patch_file="fmi-library-2.0.3-fixes.patch")
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*", src="temp-deploy")

    def package_info(self):
        self.cpp_info.libs = ["fmi"]
        
