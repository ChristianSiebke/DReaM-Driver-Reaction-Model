################################################################################
# Copyright (c) 2021 ITK Engineering GmbH
#		        2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

################################################################################
# Install file for building OSI with Conan
################################################################################

from conans import ConanFile, CMake, tools

class OsiConan(ConanFile):
    name = "OSI"
    version = "3.2.0"
    license = "<Put the package license here>"
    author = "Michael Scharfenberg michael.scharfenberg@itk-engineering.de"
    url = "https://github.com/OpenSimulationInterface"
    description = "<Description of Osi here>"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True, "protobuf:shared": True}
    generators = "cmake"

    def configure(self):
        if self.settings.os == "Linux": #For Windows it is required that Protobuf is installed via pacman or pip in MSYS2
            self.generators = "cmake_find_package", "cmake_paths"

    def source(self):
        git = tools.Git(folder="osi3")
        git.clone("https://github.com/OpenSimulationInterface/open-simulation-interface.git", "v3.2.0", "--recursive")

        if self.settings.os == "Linux":
            tools.replace_in_file("osi3/CMakeLists.txt",
                                "project(open_simulation_interface)",
                                'project(open_simulation_interface)\n' +
                                'include(${CMAKE_BINARY_DIR}/conan_paths.cmake)\n' +
                                'include_directories(${CONAN_PROTOBUF_ROOT}/include)\n')

        self.run("find . -maxdepth 1 -name '*.proto' -exec sed -i '2i option cc_enable_arenas = true;' {} \;", win_bash=True, cwd="osi3")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="osi3",
                        defs={"CMAKE_INSTALL_PREFIX":"./temp-deploy"})
        cmake.build()
        cmake.install() # cmake stays in charge of install step logic

    def package(self):
        self.copy("*", src="temp-deploy")

    def package_info(self):
        self.cpp_info.libs = ["osi3"]

