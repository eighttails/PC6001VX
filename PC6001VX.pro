#-------------------------------------------------
#
# Project created by QtCreator 2012-01-01T00:17:09
#
#-------------------------------------------------

QT       += core gui

TARGET = PC6001VX
TEMPLATE = app

INCLUDEPATH += ./src/Qt

DEFINES += USESDLTHREAD USESDLCS USESDLCRIT USESDLSEMAPHORE USESDLTIMER
QMAKE_CXXFLAGS += `sdl-config --cflags` `directfb-config --cflags`

LIBS += `sdl-config --libs`

SOURCES += \
    src/breakpoint.cpp \
    src/common.cpp \
    src/config.cpp \
    src/console.cpp \
    src/cpum.cpp \
    src/cpus.cpp \
    src/d88.cpp \
    src/debug.cpp \
    src/device.cpp \
    src/disk.cpp \
    src/error.cpp \
    src/graph.cpp \
    src/ini.cpp \
    src/intr.cpp \
    src/io.cpp \
    src/joystick.cpp \
    src/keyboard.cpp \
    src/memory.cpp \
    src/movie.cpp \
    src/p6t2.cpp \
    src/pc60.cpp \
    src/pio.cpp \
    src/psg.cpp \
    src/replay.cpp \
    src/schedule.cpp \
    src/sound.cpp \
    src/status.cpp \
    src/tape.cpp \
    src/vdg.cpp \
    src/voice.cpp \
    src/vsurface.cpp \
    src/semaphore.cpp \
    src/device/ay8910.cpp \
    src/device/mc6847.cpp \
    src/device/pd7752.cpp \
    src/device/pd8255.cpp \
    src/device/z80.cpp \
    src/device/z80-dbg.cpp \
    src/Qt/guiQt.cpp \
    src/Qt/osdQt.cpp \
    src/Qt/pc6001v.cpp \
    src/Qt/thread.cpp \
    src/Qt/configdialog.cpp \
    src/Qt/colorbutton.cpp \
    src/Qt/aboutdialog.cpp

HEADERS  += \
    src/breakpoint.h \
    src/common.h \
    src/config.h \
    src/console.h \
    src/cpum.h \
    src/cpus.h \
    src/d88.h \
    src/debug.h \
    src/device.h \
    src/disk.h \
    src/error.h \
    src/graph.h \
    src/icons.h \
    src/ini.h \
    src/intr.h \
    src/io.h \
    src/joystick.h \
    src/keyboard.h \
    src/keydef.h \
    src/log.h \
    src/memory.h \
    src/movie.h \
    src/osd.h \
    src/p6device.h \
    src/p6t2.h \
    src/pc60.h \
    src/pc6001v.h \
    src/pio.h \
    src/psg.h \
    src/replay.h \
    src/schedule.h \
    src/semaphore.h \
    src/sound.h \
    src/status.h \
    src/tape.h \
    src/thread.h \
    src/typedef.h \
    src/usrevent.h \
    src/vdg.h \
    src/voice.h \
    src/vsurface.h \
    src/device/ay8910.h \
    src/device/mc6847.h \
    src/device/pd7752.h \
    src/device/pd8255.h \
    src/device/z80.h \
    src/device/z80-cdCB.h \
    src/device/z80-cdED.h \
    src/device/z80-cdXC.h \
    src/device/z80-cdXX.h \
    src/device/z80-code.h \
    src/device/z80-tbl.h \
    src/Qt/configdialog.h \
    src/Qt/colorbutton.h \
    src/Qt/aboutdialog.h

FORMS    += \
    src/Qt/configdialog.ui \
    src/Qt/aboutdialog.ui

OTHER_FILES += \
    TODO.txt \
    src/Qt/res/PC-6001.ico \
    src/Qt/res/PC-6001mk2.ico \
    src/Qt/res/PC-6001mk2SR.ico \
    src/Qt/res/PC-6601.ico \
    src/Qt/res/PC-6601SR.ico

RESOURCES += \
    src/Qt/pc6001vx.qrc
