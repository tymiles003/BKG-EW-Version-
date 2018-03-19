
TARGET = ../bnc

CONFIG -= debug
CONFIG += release

include(src.pri)

HEADERS += app.h

SOURCES += bncmain.cpp app.cpp

#unix:!macx:QMAKE_CXXFLAGS += -m64 -Dlinux -D__i386 -D_LINUX -D_INTEL -D_USE_SCHED  -D_USE_PTHREADS -D_USE_TERMIOS
#unix:!macx:QMAKE_CFLAGS += -m64 -Dlinux -D__i386 -D_LINUX -D_INTEL -D_USE_SCHED  -D_USE_PTHREADS -D_USE_TERMIOS

#win32:QMAKE_CXXFLAGS += /Od /Zc:strictStrings-
#win32:QMAKE_CFLAGS   += /Od /Zc:strictStrings-

win32:DEFINES += WIN32_LEAN_AND_MEAN _WINNT _INTEL _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS NOMSG
#unix:!macx: PRE_TARGETDEPS += $$(EW_HOME)/$$(EW_VERSION)/lib/libew.a $$(EW_HOME)/$$(EW_VERSION)/lib/dirops_ew.o

#INCLUDEPATH += /opt/earthworm/trunk/include
#DEPENDPATH += /opt/earthworm/trunk/include

#unix:!macx:!win32:INCLUDEPATH += $$(EW_HOME)/$$(EW_VERSION)/include
#unix:!macx:!win32:DEPENDPATH += $$(EW_HOME)/$$(EW_VERSION)/include

win32:INCLUDEPATH += $$(EW_HOME)/$$(EW_VERSION)/include
win32:DEPENDPATH += $$(EW_HOME)/$$(EW_VERSION)/include

#LIBS += -L/opt/earthworm/trunk/lib -lew
#OBJECTS += /opt/earthworm/trunk/lib/dirops_ew.o #$$PWD/../../../lib/libew.a

unix:!macx:!win32:LIBS += -L$$(EW_HOME)/$$(EW_VERSION)/lib/ -lew
#unix:!macx:!win32:OBJECTS += $$(EW_HOME)/$$(EW_VERSION)/lib/dirops_ew.o $$(EW_HOME)/$$(EW_VERSION)/lib/kom.o #$$PWD/../../../lib/libew.a

win32:LIBS += -L$$(EW_HOME)/$$(EW_VERSION)/lib/ -llibew
win32:OBJECTS += $$(EW_HOME)/$$(EW_VERSION)/lib/dirops_ew.obj "C:/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/VC/Tools/MSVC/14.13.26128/lib/onecore/x86/vcruntime.lib"
