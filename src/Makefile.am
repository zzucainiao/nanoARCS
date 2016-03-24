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
				cluster.h \
				graph.h \
				graph.cpp \
				overlap_graph.cpp \
				overlap_graph.h \
				unique_edge_graph.h \
				unique_edge_graph.cpp \
				strategy.h \
				lp_strategy.h \
				lp_strategy.cpp \
				max_overlap_strategy.h \
				max_overlap_strategy.cpp \
				graph_solver.cpp \
				graph_solver.h

nanoARCS_CXXFLAGS=${LOG4CXX_CFLAGS}
nanoARCS_LDFLAGS=${LOG4CXX_LIBS} \
				-lboost_filesystem \
				-lboost_system \
				-lboost_thread \
				-lboost_iostreams

EXTRA_DIST=log4cxx.properties
