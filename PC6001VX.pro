#-------------------------------------------------
#
# Project created by QtCreator 2012-01-01T00:17:09
#
#-------------------------------------------------

QT       += core gui widgets network opengl multimedia

TARGET = PC6001VX
TEMPLATE = app

#ジョイスティックが不要な場合はコメントアウトをはずす(SDLが不要になる)
#DEFINES += NOJOYSTICK

debug:DEFINES += DEBUG
INCLUDEPATH += src/Qt src/Qt/qtsingleapplication

win32:{
#Windowsでは極力ライブラリをスタティックリンクする。Qtプラグインもスタティックライブラリとしてしてリンクする
QTPLUGIN += qico
CONFIG += exceptions
QMAKE_LFLAGS += -static -lpthread
RC_FILE = src/win32/PC6001VX.rc

!contains(DEFINES, NOJOYSTICK) {
#Windowsでは環境変数SDL_DIRにSDL2のフォルダパスを定義しておく。
QMAKE_CXXFLAGS += -I$$(SDL_DIR)/include -Dmain=SDL_main
LIBS += -L$$(SDL_DIR)/lib -lmingw32 -lSDL2main -lSDL2 -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid
}
} else {
!contains(DEFINES, NOJOYSTICK) {
QMAKE_CXXFLAGS += $$system(sdl2-config --cflags)
LIBS += $$system(sdl2-config --libs)
}
}


SOURCES += \
    src/breakpoint.cpp \
    src/config.cpp \
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
    src/device/ay8910.cpp \
    src/device/mc6847.cpp \
    src/device/pd7752.cpp \
    src/device/pd8255.cpp \
    src/device/z80.cpp \
    src/device/z80-dbg.cpp \
    src/Qt/osdQt.cpp \
    src/Qt/pc6001v.cpp \
    src/Qt/configdialog.cpp \
    src/Qt/colorbutton.cpp \
    src/Qt/aboutdialog.cpp \
    src/Qt/common.cpp \
    src/Qt/console.cpp \
    src/p6el.cpp \
    src/p6vm.cpp \
    src/Qt/qtsingleapplication/qtlocalpeer.cpp \
    src/Qt/qtsingleapplication/qtlockedfile.cpp \
    src/Qt/qtsingleapplication/qtlockedfile_unix.cpp \
    src/Qt/qtsingleapplication/qtlockedfile_win.cpp \
    src/Qt/qtsingleapplication/qtsingleapplication.cpp \
    src/Qt/qtsingleapplication/qtsinglecoreapplication.cpp \
    src/Qt/qtp6vxapplication.cpp \
    src/Qt/emulationadaptor.cpp \
    src/Qt/renderview.cpp \
    src/Qt/qtel6.cpp \
    src/Qt/semaphore.cpp \
    src/Qt/thread.cpp

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
    src/Qt/aboutdialog.h \
    src/event.h \
    src/p6el.h \
    src/p6vm.h \
    src/Qt/qtsingleapplication/QtLockedFile \
    src/Qt/qtsingleapplication/QtSingleApplication \
    src/Qt/qtsingleapplication/qtlocalpeer.h \
    src/Qt/qtsingleapplication/qtlockedfile.h \
    src/Qt/qtsingleapplication/qtsingleapplication.h \
    src/Qt/qtsingleapplication/qtsinglecoreapplication.h \
    src/Qt/qtp6vxapplication.h \
    src/Qt/emulationadaptor.h \
    src/Qt/renderview.h \
    src/Qt/qtel6.h

FORMS    += \
    src/Qt/configdialog.ui \
    src/Qt/aboutdialog.ui

OTHER_FILES += \
    src/Qt/res/PC-6001.ico \
    src/Qt/res/PC-6001mk2.ico \
    src/Qt/res/PC-6001mk2SR.ico \
    src/Qt/res/PC-6601.ico \
    src/Qt/res/PC-6601SR.ico \
    src/Qt/res/fontz12.png \
    src/win32/PC6001VX.ico \
    src/win32/PC6001VX.rc \
    COPYING \
    README.txt \
    src/Qt/res/font/fonth12.png \
    src/Qt/res/font/fontz12.png \
    src/Qt/res/background.png

RESOURCES += \
    src/Qt/pc6001vx.qrc
