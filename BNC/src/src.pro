
TARGET = ../bnc

CONFIG += debug
CONFIG -= release

include(src.pri)

HEADERS += app.h

SOURCES += bncmain.cpp app.cpp

QMAKE_CXXFLAGS += -m64 -Dlinux -D__i386 -D_LINUX -D_INTEL -D_USE_SCHED  -D_USE_PTHREADS -D_USE_TERMIOS -Wno-write-strings
QMAKE_CFLAGS += -m64 -Dlinux -D__i386 -D_LINUX -D_INTEL -D_USE_SCHED  -D_USE_PTHREADS -D_USE_TERMIOS -Wno-write-strings

INCLUDEPATH += $$(EW_HOME)/$$(EW_VERSION)/include
DEPENDPATH += $$(EW_HOME)/$$(EW_VERSION)/include

LIBS += -L$$(EW_HOME)/$$(EW_VERSION)/lib/ -lew
OBJECTS += $$(EW_HOME)/$$(EW_VERSION)/lib/dirops_ew.o $$(EW_HOME)/$$(EW_VERSION)/lib/kom.o
