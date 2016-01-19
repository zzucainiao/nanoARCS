bin_PROGRAMS=nanoARCS
nanoARCS_SOURCES=main.cpp \
				molecule.cpp \
				molecule.h \
				constant.cpp \
				constant.h \
				runner.h \
				preprocess.cpp \
				preprocess.h \
				config.h \
				fles.cpp \
				fles.h \
				category.cpp \
				category.h \
				cluster.cpp \
				cluster.h

nanoARCS_CXXFLAGS=${LOG4CXX_CFLAGS}
nanoARCS_LDFLAGS=${LOG4CXX_LIBS} \
				-lboost_filesystem \
				-lboost_system

EXTRA_DIST=log4cxx.properties
