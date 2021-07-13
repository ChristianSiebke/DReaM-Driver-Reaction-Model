from conans import ConanFile, CMake, tools

class FmiConan(ConanFile):
    name = "FMILibrary"
    version = "2.0.3"
    license = "<Put the package license here>"
    author = "Michael Scharfenberg michael.scharfenberg@itk-engineering.de"
    url = "https://github.com/modelon-community"
    description = "<Description of Fmi here>"
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
                        defs={"FMILIB_BUILD_STATIC_LIB":"OFF",
                              "FMILIB_BUILD_SHARED_LIB":"ON"})
        if self.settings.os == "Linux":
            tools.patch(patch_file="fmi-library-2.0.3-fixes.patch")
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*", src="../install")

    def package_info(self):
        self.cpp_info.libs = ["fmi"]
        
