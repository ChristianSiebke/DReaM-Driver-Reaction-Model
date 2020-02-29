SET (SOURCES stochastics.cpp
			 stochastics_implementation.cpp)
SET (HEADERS stochastics.h
			 stochastics_implementation.h
			 stochastics_global.h)


add_definitions(-DSTOCHASTICS_LIBRARY)

OPENPASS_ADD_MODULE(Stochastics "")
