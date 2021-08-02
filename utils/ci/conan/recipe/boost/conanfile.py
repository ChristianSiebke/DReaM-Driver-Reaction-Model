################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

################################################################################
# Install file for building Boost with Conan
################################################################################

from conans import ConanFile, CMake, tools
import os

class BoostConan(ConanFile):
    name = "boost"
    version = "1.76.0"
    license = "BSL-1.0"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "Boost provides free peer-reviewed portable C++ source libraries"
    topics = ("conan", "boost", "libraries", "cpp")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        git = tools.Git(folder="boost")
        git.clone("https://github.com/boostorg/boost.git", "boost-1.76.0", "--recursive")

    def build(self):
        boostPath = os.path.join(os.getcwd(), 'boost')
        boostBuildPath = os.path.join(boostPath, 'tools/build')
        os.chdir(boostBuildPath)
        os.environ["PATH"] += os.pathsep + boostPath
        os.system("sh ./bootstrap.sh gcc")
        os.system("./b2.exe install")

    def package(self):
        self.copy("*.h", dst="include", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["boost"]
