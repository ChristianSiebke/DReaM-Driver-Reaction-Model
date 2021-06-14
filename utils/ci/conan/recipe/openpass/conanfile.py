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
    generators = "cmake_find_package", "cmake_paths"
    exports_sources = "../../../../../*" # use source of the repo
    short_paths = True

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC   

    def requirements(self):
        if self.options.Gui_only == False:
            #self.requires("boost/1.76.0@openpass/testing")
            #self.requires("protobuf/3.15.5")
            self.requires("osi/3.2.0@openpass/testing")
            self.requires("fmi/2.0.3@openpass/testing")
            #self.requires("gtest/1.10.0")

    #def source(self):
        #git = tools.Git(folder="openpass")
        #git.clone("https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass.git", "servant", "--recursive")


    def build(self):
        PATH_MINGW = "/mingw64/bin;"
        PATH_FMIL = ""
        PATH_BOOST = ""
        PATH_OSI = ""
        PATH_PROTOBUF = ""
        PATH_GTEST = ""

        arguments = {   "WITH_GUI":"OFF",
                        "WITH_SIMCORE":"OFF",
                        "WITH_TESTS":"OFF",
                        "WITH_DOC":"OFF",
                        "WITH_PROTOBUF_ARENA":"OFF",
                        "WITH_DEBUG_POSTFIX":"OFF",
                        "CMAKE_PREFIX_PATH": PATH_MINGW + ";"
                                            + PATH_FMIL + ";"
                                            + PATH_BOOST + ";"
                                            + PATH_OSI + ";"
                                            + PATH_PROTOBUF + ";"
                                            + PATH_GTEST
                    }
        cmake = CMake(self)

        if self.options.Gui_only:
            arguments["WITH_GUI"] = "ON"
        else:
            arguments = {   "WITH_GUI":"OFF",
                            "WITH_SIMCORE":"ON",
                            "WITH_TESTS":"OFF",
                            "WITH_DOC":"OFF",
                            "WITH_PROTOBUF_ARENA":"OFF",
                            "WITH_DEBUG_POSTFIX":"OFF",
                        }

        cmake.configure(defs=arguments)
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="openpass")
        self.copy("*.exe", dst="bin", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.html", dst="doc")

    def package_info(self):
        self.cpp_info.libs = ["openpass"]

