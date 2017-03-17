includedir = @includedir@/JSBSim/models/thermal

LIBRARY_SOURCES = Thermals.cpp Windfield.cpp

LIBRARY_INCLUDES = Thermals.hpp Windfield.cpp

if BUILD_LIBRARIES
noinst_LTLIBRARIES = libThermal.la
include_HEADERS = $(LIBRARY_INCLUDES)
libThermal_la_SOURCES = $(LIBRARY_SOURCES)
libThermal_la_CXXFLAGS = $(AM_CXXFLAGS)
else
noinst_LIBRARIES = libThermal.a
noinst_HEADERS = $(LIBRARY_INCLUDES)
libThermal_a_SOURCES = $(LIBRARY_SOURCES)
endif

INCLUDES = -I$(top_srcdir)/src -I../../
