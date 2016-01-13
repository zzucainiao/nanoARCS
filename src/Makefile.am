bin_PROGRAMS=nanoARCS
nanoARCS_SOURCES=main.cpp \
				molecule.cpp \
				molecule.h \
				constant.cpp \
				constant.h

nanoARCS_CXXFLAGS=${LOG4CXX_CFLAGS}
nanoARCS_LDFLAGS=${LOG4CXX_LIBS} \
				-lboost_filesystem \
				-lboost_system

EXTRA_DIST=log4cxx.properties
