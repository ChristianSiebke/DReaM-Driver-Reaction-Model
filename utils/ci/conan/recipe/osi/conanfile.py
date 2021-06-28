from conans import ConanFile, CMake, tools

class OsiConan(ConanFile):
    name = "OSI"
    version = "3.2.0"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Osi here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True, "protobuf:shared": True}
    generators = "cmake"
    # short_paths = True

    def configure(self):
        if self.settings.os == "Linux": #For Windows it is required that Protobuf is installed via pacman or pip in MSYS2
            self.generators = "cmake_find_package", "cmake_paths"
    
    def requirements(self):
        if self.settings.os == "Linux":
            self.requires("protobuf/3.11.4")

    def source(self):
        git = tools.Git(folder="osi3")
        git.clone("https://github.com/OpenSimulationInterface/open-simulation-interface.git", "v3.2.0", "--recursive")

        if self.settings.os == "Linux":
            tools.replace_in_file("osi3/CMakeLists.txt",
                                "project(open_simulation_interface)",
                                'project(open_simulation_interface)\n' +
                                'include(${CMAKE_BINARY_DIR}/conan_paths.cmake)\n' +
                                'include_directories(${CONAN_PROTOBUF_ROOT}/include)\n')

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="osi3")
        cmake.build()
        #cmake.install()

    def package(self):
        # self.copy("*", src="../package")
        # self.copy("*.dll", src="../package", keep_path=False)
        # self.copy("*.lib", src="../package", keep_path=False)
        # self.copy("*.a", src="../package", keep_path=False)
        self.copy("*.h", dst="include/osi3")
        self.copy("*.dll", dst="lib/osi3", keep_path=False)
        self.copy("*.lib", dst="lib/osi3", keep_path=False)
        self.copy("*.a", dst="lib/osi3", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["osi3"]

