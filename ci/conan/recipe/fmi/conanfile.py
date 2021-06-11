from conans import ConanFile, CMake, tools


class FmiConan(ConanFile):
    name = "fmi"
    version = "2.0.3"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Fmi here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    exports_sources = "fmil203.patch"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self): 
        git = tools.Git(folder="fmi")
        git.clone("https://github.com/modelon-community/fmi-library.git", "2.0.3", "--recursive")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="fmi",
                        defs={"FMILIB_BUILD_STATIC_LIB":"OFF",
                              "FMILIB_BUILD_SHARED_LIB":"ON"})
        if self.settings.os == "Linux":
            tools.patch(patch_file="fmil203.patch")
        cmake.build()

    def package(self):
        # self.copy("*.h", dst="include", keep_path=False)
        # self.copy("*.lib", dst="lib", keep_path=False)
        # self.copy("*.dll", dst="lib", keep_path=False)
        # self.copy("*.so", dst="lib", keep_path=False)
        # self.copy("*.dylib", dst="lib", keep_path=False)
        # self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*", dst="include/FMILibrary", src= "FMI")
        self.copy("*", dst="include/FMILibrary", src= "FMI2")
        self.copy("*", dst="include/FMILibrary", src= "JM")
        self.copy("fmilib.h", dst="include/FMILibrary")
        self.copy("fmilib_config.h", dst="include/FMILibrary")

    def package_info(self):
        self.cpp_info.libs = ["fmi"]
        

