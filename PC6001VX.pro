#-------------------------------------------------
#
# Project created by QtCreator 2012-01-01T00:17:09
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = PC6001VX
TEMPLATE = app
VERSION = 4.2.12

QMAKE_TARGET_COMPANY = eighttails
QMAKE_TARGET_DESCRIPTION = PC6001VX
QMAKE_TARGET_COPYRIGHT = eighttails
QMAKE_TARGET_PRODUCT = PC6001VX

TRANSLATIONS += \
    src/Qt/translation/PC6001VX_en.ts \
    src/Qt/translation/PC6001VX_ru.ts
TR_EXCLUDE += /usr/include/*
CONFIG += lrelease embed_translations

CONFIG += silent
CONFIG += link_prl link_pkgconfig c++17
gcc:QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable
clang:QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable

#Define for Qt dependent code
DEFINES += QTP6VX
DEFINES += USEFILESYSTEM

#Use FMGEN for PSG(For non commercial only)
#DEFINES += USEFMGEN

#Disable joystick support. If uncommented below, PC6001VX does not depend on SDL2.
#DEFINES += NOJOYSTICK

#Disable video capture support. If uncommented below, PC6001VX does not depend on ffmpeg.
#DEFINES += NOAVI

#Disable debug features. For low performance machine.
#DEFINES += NOMONITOR

#Set if timer resolutions of target OS is more than 1ms.
#DEFINES += DELAY_TIMER_MS_DEFAULT=10

#Other feature control flags.
#DEFINES += NOSINGLEAPP
#DEFINES += NOOPENGL
#DEFINES += NOSOUND
#DEFINES += NOCALLBACK
#DEFINES += NOLIBINTL
#DEFINES += AUTOSUSPEND
#DEFINES += REPLAYDEBUG_FRAME
#DEFINES += REPLAYDEBUG_INST

debug:DEFINES += DEBUG
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
INCLUDEPATH += src src/Qt src/Qt/babel


#Configuration for UNIX variants
unix:!macx {
    #Configuration for Android
    android {
        ANDROID_TARGET_SDK_VERSION = 34
        DEFINES += NOLIBINTL NOSINGLEAPP NOMONITOR NOAVI NOOPENGL ALWAYSFULLSCREEN AUTOSUSPEND
        DEFINES -= QT_NO_DEBUG_OUTPUT USEFILESYSTEM
        #Set "ROM Path in target device" to "CUSTOM_ROM_PATH environment variable on build host"
        debug:DEFINES += CUSTOMROMPATH=\\\"$$(CUSTOM_ROM_PATH)\\\"
        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

        DISTFILES += \
            android/AndroidManifest.xml

        HEADERS +=
        SOURCES +=
    }

    ios {
        DEFINES += NOJOYSTICK NOAVI ALWAYSFULLSCREEN
    }

    !android:!ios {
        #Configuration for X11(XCB)
        DEFINES += USE_X11
        PKGCONFIG += x11
    }
}

#Configuration for Windows
win32 {
    #On Windows, link libraries statically as long as possible.
    QMAKE_LFLAGS += -static
    LIBS_PRIVATE= -lpthread -lsetupapi -lOleAut32
    PKG_CONFIG = 'pkg-config --static'
    RC_ICONS += src/win32/PC6001VX.ico    
} else {
    DEFINES += __stdcall=
}

#Find library to handle joysticks.
!contains(DEFINES, NOJOYSTICK) {
    packagesExist(sdl2) {
        win32:PKGCONFIG += sdl2_withqt
        else:PKGCONFIG += sdl2
    } else {
        DEFINES += NOJOYSTICK
    }
}

!contains(DEFINES, NOOPENGL) {
    QT += opengl openglwidgets
}

!contains(DEFINES, NOSOUND) {
    QT += multimedia
    SOURCES += \
        src/Qt/wavfile.cpp
}

!contains(DEFINES, NOAVI) {
    win32 {
        FFMPEG_VER = 7.0.1
        packagesExist(libavformat-private$${FFMPEG_VER}) {
            DEFINES += __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS
            PKGCONFIG += libavformat-private$${FFMPEG_VER} libavcodec-private$${FFMPEG_VER} libswscale-private$${FFMPEG_VER} \
                        libswresample-private$${FFMPEG_VER} libavutil-private$${FFMPEG_VER}
        } else {
            DEFINES += NOAVI
        }
    } else {
        packagesExist(libavformat) {
            DEFINES += __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS
            PKGCONFIG += libavformat libavcodec libswscale libswresample libavutil
        } else {
            DEFINES += NOAVI
        }
    }
}

!contains(DEFINES, NOSINGLEAPP) {
    include(src/Qt/SingleApplication/singleapplication.pri)
    DEFINES += QAPPLICATION_CLASS=QApplication
}

!contains(DEFINES, NOLIBINTL) {
    macx:LIBS_PRIVATE += -lintl
    win32:LIBS_PRIVATE += -lintl -liconv
}

SOURCES += \
    src/Qt/audiooutputwrapper.cpp \
    src/Qt/colorbutton.cpp \
    src/Qt/configdialog.cpp \
    src/Qt/cthread.cpp \
    src/Qt/emulationadaptor.cpp \
    src/Qt/keypanel.cpp \
    src/Qt/keypanelbutton.cpp \
    src/Qt/keystatewatcher.cpp \
    src/Qt/mainwidget.cpp \
    src/Qt/normalvirtualkeyboardscene.cpp \
    src/Qt/osdQt.cpp \
    src/Qt/p6vxapp.cpp \
    src/Qt/p6vxcommon.cpp \
    src/Qt/pc6001v.cpp \
    src/Qt/qtel6.cpp \
    src/Qt/renderview.cpp \
    src/Qt/simplevirtualkeyboardscene.cpp \
    src/Qt/systeminfodialog.cpp \
    src/Qt/virtualkeyboardscene.cpp \
    src/Qt/virtualkeyboardview.cpp \
    src/Qt/virtualkeyitem.cpp \
    src/Qt/virtualkeytabwidget.cpp \
    src/Qt/virtualstickitem.cpp \
    src/breakpoint.cpp \
    src/common.cpp \
    src/config.cpp \
    src/console.cpp \
    src/cpum.cpp \
    src/cpus.cpp \
    src/csemaphore.cpp \
    src/d88.cpp \
    src/debug.cpp \
    src/device.cpp \
    src/device/ay8910.cpp \
    src/device/fmgen/fmgen.cpp \
    src/device/fmgen/fmtimer.cpp \
    src/device/fmgen/opm.cpp \
    src/device/fmgen/opna.cpp \
    src/device/fmgen/psg.cpp \
    src/device/mc6847.cpp \
    src/device/pd7752.cpp \
    src/device/pd8255.cpp \
    src/device/ym2203.cpp \
    src/device/z80-dbg.cpp \
    src/device/z80.cpp \
    src/disk.cpp \
    src/error.cpp \
    src/graph.cpp \
    src/ini.cpp \
    src/intr.cpp \
    src/io.cpp \
    src/joystick.cpp \
    src/keyboard.cpp \
    src/memblk.cpp \
    src/memory.cpp \
    src/movie.cpp \
    src/p6el.cpp \
    src/p6t2.cpp \
    src/p6vm.cpp \
    src/pio.cpp \
    src/psgfm.cpp \
    src/replay.cpp \
    src/romaji.cpp \
    src/schedule.cpp \
    src/sound.cpp \
    src/status.cpp \
    src/tape.cpp \
    src/vdg.cpp \
    src/voice.cpp \
    src/vsurface.cpp

HEADERS  += \
    src/Qt/audiooutputwrapper.h \
    src/Qt/colorbutton.h \
    src/Qt/configdialog.h \
    src/Qt/cthread.h \
    src/Qt/emulationadaptor.h \
    src/Qt/keypanel.h \
    src/Qt/keypanelbutton.h \
    src/Qt/keystatewatcher.h \
    src/Qt/mainwidget.h \
    src/Qt/normalvirtualkeyboardscene.h \
    src/Qt/p6vxapp.h \
    src/Qt/qtel6.h \
    src/Qt/qtutil.h \
    src/Qt/renderview.h \
    src/Qt/simplevirtualkeyboardscene.h \
    src/Qt/systeminfodialog.h \
    src/Qt/virtualkeyboardscene.h \
    src/Qt/virtualkeyboardview.h \
    src/Qt/virtualkeyitem.h \
    src/Qt/virtualkeytabwidget.h \
    src/Qt/virtualstickitem.h \
    src/Qt/wavfile.h \
    src/breakpoint.h \
    src/common.h \
    src/config.h \
    src/console.h \
    src/cpum.h \
    src/cpus.h \
    src/csemaphore.h \
    src/d88.h \
    src/debug.h \
    src/device.h \
    src/device/ay8910.h \
    src/device/fmgen/diag.h \
    src/device/fmgen/fmgen.h \
    src/device/fmgen/fmgeninl.h \
    src/device/fmgen/fmtimer.h \
    src/device/fmgen/headers.h \
    src/device/fmgen/misc.h \
    src/device/fmgen/opm.h \
    src/device/fmgen/opna.h \
    src/device/fmgen/psg.h \
    src/device/fmgen/types.h \
    src/device/mc6847.h \
    src/device/pd7752.h \
    src/device/pd8255.h \
    src/device/psgbase.h \
    src/device/ym2203.h \
    src/device/z80-cdCB.h \
    src/device/z80-cdED.h \
    src/device/z80-cdXC.h \
    src/device/z80-cdXX.h \
    src/device/z80-code.h \
    src/device/z80-tbl.h \
    src/device/z80.h \
    src/disk.h \
    src/error.h \
    src/event.h \
    src/graph.h \
    src/id_menu.h \
    src/ini.h \
    src/intr.h \
    src/io.h \
    src/joystick.h \
    src/keyboard.h \
    src/keydef.h \
    src/log.h \
    src/memblk.h \
    src/memory.h \
    src/movie.h \
    src/osd.h \
    src/p6el.h \
    src/p6t2.h \
    src/p6vm.h \
    src/p6vxcommon.h \
    src/pc6001v.h \
    src/pio.h \
    src/psgfm.h \
    src/replay.h \
    src/romaji.h \
    src/schedule.h \
    src/sound.h \
    src/status.h \
    src/tape.h \
    src/typedef.h \
    src/vdg.h \
    src/voice.h \
    src/vsurface.h

FORMS    += \
    src/Qt/configdialog.ui \
    src/Qt/systeminfodialog.ui \
    src/Qt/virtualkeytabwidget.ui

OTHER_FILES += \
    android/AndroidManifest.xml

RESOURCES += \
    src/Qt/pc6001vx.qrc

DISTFILES += \
    LICENSE \
    android/AndroidManifest.xml \
    android/PrivacyPolicy.html \
    android/res/xml/filepaths.xml


