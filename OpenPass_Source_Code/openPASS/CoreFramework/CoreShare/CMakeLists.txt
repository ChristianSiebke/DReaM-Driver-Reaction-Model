SET (SOURCES callbacks.cpp
			 log.cpp
			 parameters.cpp
			 xmlParser.cpp
			 cephesMIT/const.c
			 cephesMIT/fresnl.c
			 cephesMIT/polevl.c)
SET (HEADERS callbacks.h
			 log.h
			 parameters.h
			 xmlParser.h
			 cephesMIT/mconf.h)



OPENPASS_add_library(CoreShare STATIC)
set_target_properties(CoreShare PROPERTIES FOLDER "CoreFramework")

qt5_use_modules(CoreShare Xml)