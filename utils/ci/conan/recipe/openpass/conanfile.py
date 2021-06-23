from conans import ConanFile, CMake, tools


class OpenpassConan(ConanFile):
    name = "openpass"
    version = "0.8"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Openpass here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "Gui_only": [True, False]}
    default_options = {"shared": True, "fPIC": True, "boost:shared": True, "Gui_only": False}
    generators = "cmake"
    # build_folder=""
    exports_sources = "../../../../../*" # use source of the repo
    short_paths = True

    # def configure(self):
    #     if self.settings.os == "Windows":
    #         self.build_folder = "C:/tmp/op"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC   

    def requirements(self):
        if self.options.Gui_only == False:
            #self.requires("boost/1.76.0@openpass/testing")
            #self.requires("protobuf/3.15.5")
            self.requires("OSI/3.2.0@openpass/testing")
            self.requires("FMILibrary/2.0.3@openpass/testing")
            #self.requires("gtest/1.10.0")
    
    def imports(self):
        self.copy("*", dst="ThirdParty")

    # def source(self):
    #     git = tools.Git()
    #     git.clone("https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git", "servant", "--recursive")

    def build(self):
        cmake = CMake(self)
        cmake.configure(defs={"CMAKE_PREFIX_PATH": "./ThirdParty"})
        cmake.build()

    def package(self):
        self.copy("*.exe", dst="bin", keep_path=False)
        self.copy("*.dll", dst="bin/lib", keep_path=False)
        self.copy("*.a", dst="bin/lib", keep_path=False)
        self.copy("*.so", dst="bin/lib", keep_path=False)
        self.copy("*.html", dst="doc")

    def package_info(self):
        self.cpp_info.libs = ["openpass"]
