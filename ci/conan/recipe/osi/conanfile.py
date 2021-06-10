from conans import ConanFile, CMake, tools


class OsiConan(ConanFile):
    name = "osi"
    version = "3.2.0"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Osi here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    generators = "cmake"
    requires = "protobuf/3.15.5"
    short_paths = True

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        git = tools.Git(folder="osi3")
        git.clone("https://github.com/OpenSimulationInterface/open-simulation-interface.git", "v3.2.0", "--recursive")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="osi3")
        cmake.build()

    def package(self):
        self.copy("*.h", dst=".", keep_path=False)
        self.copy("*.cc", dst=".", keep_path=False)
        self.copy("*.dll", dst=".", keep_path=False)
        self.copy("*.lib", dst=".", keep_path=False)
        self.copy("*.so", dst=".", keep_path=False)
        self.copy("*.a", dst=".", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["osi3"]

