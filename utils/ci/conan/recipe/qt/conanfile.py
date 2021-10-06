################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

################################################################################
# Install file for building Qt with Conan
################################################################################

from conans import ConanFile, CMake, tools
import os

class QtConan(ConanFile):
    name = "qt"
    version = "5.15.2"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Qt here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    no_copy_build = True

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        git = tools.Git(folder="qt")
        git.clone("https://github.com/qt/qtbase.git", "v5.15.2", "--recursive")
        git.run("submodule init")
        git.run("submodule update")

    def build(self):
        os.system("./qt/configure -h")
        os.system("./qt/configure -platform win32-g++ -opensource -confirm-license -mp")
        os.system("make")

    def package(self):
        self.copy("*.h", dst="include", src="qt")
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["qt"]

