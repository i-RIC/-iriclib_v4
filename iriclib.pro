TEMPLATE = lib
TARGET = iriclib
DEPENDPATH += .
win32 {
	INCLUDEPATH += "E:/iricdev_2013/lib/install/cgnslib-3.2.1/debug/include"
}
unix {
	INCLUDEPATH += /usr/include
}
CONFIG += dll

DEFINES += IRICLIBDLL_LIBRARY

QT =
DEFINES += UPPERCASE

include( ../paths.pri )

win32 {
	LIBS += -lcgnsdll
}
unix {
	LIBS += -lcgns
	LIBS += -lhdf5
}

#installation settings

unix {
  inst_headers.path = /usr/local/iRIC/include
  inst_headers.files = iriclib.h iriclib_global.h iriclib_f.h iriclib_pointmap.h iriclib_riversurvey.h iriclib_polygon.h iriclib_bstream.h
	INSTALLS += inst_headers

	target.path = /usr/local/iRIC/lib
	INSTALLS += target
}

# Input
HEADERS += error_macros.h \
           filelocker.h \
           fortran_macros.h \
           iriclib.h \
           iriclib_bstream.h \
           iriclib_cgnsfile.h \
           iriclib_global.h \
           iriclib_pointmap.h \
           iriclib_polygon.h \
           iriclib_polyline.h \
           iriclib_riversurvey.h \
           iriclib_wrapper_api.h \
           private/filelocker_impl.h \
           private/iriclib_cgnsfile_baseiterativet.h \
           private/iriclib_cgnsfile_baseiterativet_detail.h \
           private/iriclib_cgnsfile_impl.h \
           private/iriclib_cgnsfile_solutionwriter.h \
           private/iriclib_cgnsfile_solutionwriterdividesolutions.h \
           private/iriclib_cgnsfile_solutionwriterstandard.h
SOURCES += filelocker.cpp \
           iric_ftoc.c \
           iriclib.cpp \
           iriclib_bstream.cpp \
           iriclib_cgnsfile_base.cpp \
           iriclib_cgnsfile_bc.cpp \
           iriclib_cgnsfile_cc.cpp \
           iriclib_cgnsfile_complex_cc.cpp \
           iriclib_cgnsfile_geo.cpp \
           iriclib_cgnsfile_grid.cpp \
           iriclib_cgnsfile_sol.cpp \
           iriclib_geo.cpp \
           iriclib_pointmap.cpp \
           iriclib_polygon.cpp \
           iriclib_polyline.cpp \
           iriclib_riversurvey.cpp \
           iriclib_single.c \
           iriclib_wrapper_api.cpp \
           private/iriclib_cgnsfile_solutionwriter.cpp \
           private/iriclib_cgnsfile_solutionwriterdividesolutions.cpp \
           private/iriclib_cgnsfile_solutionwriterstandard.cpp
