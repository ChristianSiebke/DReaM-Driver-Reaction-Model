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
# Install file for building OpenPASS with Conan
################################################################################

from conans import ConanFile, CMake, tools
from pathlib import Path
import shutil

class OpenpassConan(ConanFile):
    name = "openpass"
    version = "0.8"
    license = "Eclipse Public License 2.0"
    author = "Michael Scharfenberg michael.scharfenberg@itk-engineering.de"
    url = "https://gitlab.eclipse.org/eclipse/simopenpass"
    description = "OpenPASS is an open source platform for effectiveness assessment of advanced driver assistance systems and automated driving"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "Gui_only": [True, False]}
    default_options = {"shared": True, "fPIC": True, "boost:shared": True, "Gui_only": False}
    generators = "cmake"
    exports_sources = "../../../../../*" # use source of the repo
    short_paths = True

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC   

    def requirements(self):
        #if self.options.Gui_only == False:
            #self.requires("boost/1.76.0@openpass/testing")
            #self.requires("protobuf/3.15.5")
            self.requires("OSI/3.2.0")
            self.requires("FMILibrary/2.0.3")
            #self.requires("gtest/1.10.0")
    
    def imports(self):
        self.copy("*", dst="ThirdParty")

    # def source(self):
    #     git = tools.Git()
    #     git.clone("https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git", "servant", "--recursive")

    def build(self):
        cmake = CMake(self)
        if self.settings.os == "Windows":
            cmake.configure(defs={"CMAKE_PREFIX_PATH":"ThirdParty",
                                "CMAKE_INSTALL_PREFIX":"temp-deploy",
                                "WITH_GUI":"OFF",
                                "WITH_TESTS":"OFF",
                                "INSTALL_EXTRA_RUNTIME_DEPS":"ON"})
        elif self.settings.os == "Linux":
            cmake.configure(defs={"CMAKE_PREFIX_PATH":"./ThirdParty",
                                "CMAKE_INSTALL_PREFIX":"./temp-deploy",
                                "CMAKE_C_COMPILER":"gcc-10",
                                "CMAKE_CXX_COMPILER":"g++-10",
                                "WITH_GUI":"OFF",
                                "WITH_TESTS":"OFF",
                                "INSTALL_EXTRA_RUNTIME_DEPS":"ON"})
        cmake.build()
        # can be deleted as soon as branch is up to date
        if self.settings.os == "Windows":
            self.run("mingw32-make doc", win_bash=True)
        elif self.settings.os == "Linux":
            self.run("make doc", win_bash=True)
        cmake.install()

    def package(self):
        self.copy("*", src="temp-deploy", excludes=["bin/*"])
        self.copy("*", src="temp-deploy/bin")

    def package_info(self):
        self.cpp_info.libs = ["openpass"]
