cmake_minimum_required(VERSION 3.16)

project(PC6001VX VERSION 4.3.1 LANGUAGES CXX)

if(WIN32)
    set(CMAKE_FIND_PACKAGE_PREFER_STATIC_LIBS TRUE)
    set(CMAKE_FIND_LIBRARY_SUFFIXES_OVERRIDE ".a")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(PKG_CONFIG_ARGN --static CACHE STRING "Arguments to supply to pkg-config" FORCE)
    set(OPENSSL_USE_STATIC_LIBS ON)
    set(ZLIB_USE_STATIC_LIBS ON)

    function(pc6001vx_collect_qt_multimedia_pri out_var seed_path)
        if(NOT seed_path OR NOT EXISTS "${seed_path}")
            return()
        endif()

        set(candidates
            "${seed_path}/share/qt6/mkspecs/modules/qt_lib_multimedia_private.pri"
        )

        set(current_path "${seed_path}")
        foreach(_range 1 2 3)
            get_filename_component(current_path "${current_path}" DIRECTORY)
            list(APPEND candidates
                "${current_path}/share/qt6/mkspecs/modules/qt_lib_multimedia_private.pri"
            )
        endforeach()

        set(${out_var} "${${out_var}};${candidates}" PARENT_SCOPE)
    endfunction()

    if(NOT DEFINED FFMPEG_DIR OR FFMPEG_DIR STREQUAL "")
        set(PC6001VX_QT_MULTIMEDIA_PRI_CANDIDATES)

        if(DEFINED QT_QMAKE_EXECUTABLE AND EXISTS "${QT_QMAKE_EXECUTABLE}")
            get_filename_component(PC6001VX_QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
            get_filename_component(PC6001VX_QT_PREFIX_DIR "${PC6001VX_QT_BIN_DIR}" DIRECTORY)
            pc6001vx_collect_qt_multimedia_pri(PC6001VX_QT_MULTIMEDIA_PRI_CANDIDATES "${PC6001VX_QT_PREFIX_DIR}")
        endif()

        if(DEFINED Qt6_DIR AND EXISTS "${Qt6_DIR}")
            pc6001vx_collect_qt_multimedia_pri(PC6001VX_QT_MULTIMEDIA_PRI_CANDIDATES "${Qt6_DIR}")
        endif()

        foreach(PC6001VX_CMAKE_PREFIX IN LISTS CMAKE_PREFIX_PATH)
            pc6001vx_collect_qt_multimedia_pri(PC6001VX_QT_MULTIMEDIA_PRI_CANDIDATES "${PC6001VX_CMAKE_PREFIX}")
        endforeach()

        list(REMOVE_DUPLICATES PC6001VX_QT_MULTIMEDIA_PRI_CANDIDATES)

        foreach(PC6001VX_QT_MULTIMEDIA_PRI IN LISTS PC6001VX_QT_MULTIMEDIA_PRI_CANDIDATES)
            if(EXISTS "${PC6001VX_QT_MULTIMEDIA_PRI}")
                file(STRINGS "${PC6001VX_QT_MULTIMEDIA_PRI}" PC6001VX_QMAKE_LIBDIR_FFMPEG_LINE
                    REGEX "^QMAKE_LIBDIR_FFMPEG = "
                )
                if(PC6001VX_QMAKE_LIBDIR_FFMPEG_LINE)
                    string(REGEX REPLACE "^QMAKE_LIBDIR_FFMPEG = " "" PC6001VX_QMAKE_LIBDIR_FFMPEG
                        "${PC6001VX_QMAKE_LIBDIR_FFMPEG_LINE}"
                    )
                    get_filename_component(PC6001VX_DETECTED_FFMPEG_DIR
                        "${PC6001VX_QMAKE_LIBDIR_FFMPEG}" DIRECTORY
                    )
                    if(EXISTS "${PC6001VX_DETECTED_FFMPEG_DIR}")
                        set(FFMPEG_DIR "${PC6001VX_DETECTED_FFMPEG_DIR}" CACHE PATH
                            "Path to FFmpeg installation" FORCE
                        )
                        list(PREPEND CMAKE_PREFIX_PATH "${FFMPEG_DIR}")
                        message(STATUS "Auto-detected FFMPEG_DIR from Qt metadata: ${FFMPEG_DIR}")
                        break()
                    endif()
                endif()
            endif()
        endforeach()
    endif()

    get_filename_component(PC6001VX_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(PC6001VX_MINGW_ROOT_DIR "${PC6001VX_MINGW_BIN_DIR}" DIRECTORY)
    file(GLOB PC6001VX_GCC_LIB_DIR_CANDIDATES LIST_DIRECTORIES TRUE
        "${PC6001VX_MINGW_ROOT_DIR}/lib/gcc/*/${CMAKE_CXX_COMPILER_VERSION}"
    )
    list(GET PC6001VX_GCC_LIB_DIR_CANDIDATES 0 PC6001VX_GCC_LIB_DIR)

    find_file(PC6001VX_STATIC_STDCXX
        NAMES libstdc++.a
        PATHS "${PC6001VX_MINGW_ROOT_DIR}/lib"
        NO_DEFAULT_PATH
        REQUIRED
    )
    find_file(PC6001VX_STATIC_GCC
        NAMES libgcc.a
        PATHS "${PC6001VX_GCC_LIB_DIR}"
        NO_DEFAULT_PATH
        REQUIRED
    )
    find_file(PC6001VX_STATIC_GCC_EH
        NAMES libgcc_eh.a
        PATHS "${PC6001VX_GCC_LIB_DIR}"
        NO_DEFAULT_PATH
        REQUIRED
    )

endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Multimedia OpenGL OpenGLWidgets)
find_package(Qt6 COMPONENTS LinguistTools QUIET)

qt_standard_project_setup()

set(QAPPLICATION_CLASS QApplication)
set(QT_DEFAULT_MAJOR_VERSION 6)

add_subdirectory(src/Qt/SingleApplication)

set(PC6001VX_SOURCES
    src/Qt/audiooutputwrapper.cpp
    src/Qt/colorbutton.cpp
    src/Qt/configdialog.cpp
    src/Qt/cthread.cpp
    src/Qt/emulationadaptor.cpp
    src/Qt/keypanel.cpp
    src/Qt/keypanelbutton.cpp
    src/Qt/keystatewatcher.cpp
    src/Qt/mainwidget.cpp
    src/Qt/normalvirtualkeyboardscene.cpp
    src/Qt/osdQt.cpp
    src/Qt/p6vxapp.cpp
    src/Qt/p6vxcommon.cpp
    src/Qt/pc6001v.cpp
    src/Qt/qtel6.cpp
    src/Qt/renderview.cpp
    src/Qt/simplevirtualkeyboardscene.cpp
    src/Qt/systeminfodialog.cpp
    src/Qt/virtualkeyboardscene.cpp
    src/Qt/virtualkeyboardview.cpp
    src/Qt/virtualkeyitem.cpp
    src/Qt/virtualkeytabwidget.cpp
    src/Qt/virtualstickitem.cpp
    src/Qt/wavfile.cpp
    src/breakpoint.cpp
    src/common.cpp
    src/config.cpp
    src/console.cpp
    src/cpum.cpp
    src/cpus.cpp
    src/csemaphore.cpp
    src/d88.cpp
    src/debug.cpp
    src/device.cpp
    src/device/ay8910.cpp
    src/device/fmgen/fmgen.cpp
    src/device/fmgen/fmtimer.cpp
    src/device/fmgen/opm.cpp
    src/device/fmgen/opna.cpp
    src/device/fmgen/psg.cpp
    src/device/mc6847.cpp
    src/device/pd7752.cpp
    src/device/pd8255.cpp
    src/device/ym2203.cpp
    src/device/z80-dbg.cpp
    src/device/z80.cpp
    src/disk.cpp
    src/error.cpp
    src/graph.cpp
    src/ini.cpp
    src/intr.cpp
    src/io.cpp
    src/joystick.cpp
    src/keyboard.cpp
    src/memblk.cpp
    src/memory.cpp
    src/movie.cpp
    src/p6el.cpp
    src/p6t2.cpp
    src/p6vm.cpp
    src/pio.cpp
    src/psgfm.cpp
    src/replay.cpp
    src/romaji.cpp
    src/schedule.cpp
    src/sound.cpp
    src/status.cpp
    src/tape.cpp
    src/vdg.cpp
    src/voice.cpp
    src/vsurface.cpp
)

set(PC6001VX_HEADERS
    src/Qt/audiooutputwrapper.h
    src/Qt/babel/babel.cpp
    src/Qt/babel/babel.h
    src/Qt/colorbutton.h
    src/Qt/configdialog.h
    src/Qt/cthread.h
    src/Qt/emulationadaptor.h
    src/Qt/keypanel.h
    src/Qt/keypanelbutton.h
    src/Qt/keystatewatcher.h
    src/Qt/mainwidget.h
    src/Qt/normalvirtualkeyboardscene.h
    src/Qt/p6vxapp.h
    src/Qt/qtel6.h
    src/Qt/qtutil.h
    src/Qt/renderview.h
    src/Qt/simplevirtualkeyboardscene.h
    src/Qt/systeminfodialog.h
    src/Qt/virtualkeyboardscene.h
    src/Qt/virtualkeyboardview.h
    src/Qt/virtualkeyitem.h
    src/Qt/virtualkeytabwidget.h
    src/Qt/virtualstickitem.h
    src/Qt/wavfile.h
    src/breakpoint.h
    src/common.h
    src/config.h
    src/console.h
    src/cpum.h
    src/cpus.h
    src/csemaphore.h
    src/d88.h
    src/debug.h
    src/device.h
    src/device/ay8910.h
    src/device/fmgen/diag.h
    src/device/fmgen/file.h
    src/device/fmgen/fmgen.h
    src/device/fmgen/fmgeninl.h
    src/device/fmgen/fmtimer.h
    src/device/fmgen/headers.h
    src/device/fmgen/misc.h
    src/device/fmgen/opm.h
    src/device/fmgen/opna.h
    src/device/fmgen/psg.h
    src/device/fmgen/types.h
    src/device/mc6847.h
    src/device/pd7752.h
    src/device/pd8255.h
    src/device/psgbase.h
    src/device/ym2203.h
    src/device/z80-cdCB.h
    src/device/z80-cdED.h
    src/device/z80-cdXC.h
    src/device/z80-cdXX.h
    src/device/z80-code.h
    src/device/z80-tbl.h
    src/device/z80.h
    src/disk.h
    src/error.h
    src/event.h
    src/graph.h
    src/id_menu.h
    src/ini.h
    src/intr.h
    src/io.h
    src/joystick.h
    src/keyboard.h
    src/keydef.h
    src/log.h
    src/memblk.h
    src/memory.h
    src/movie.h
    src/osd.h
    src/p6el.h
    src/p6t2.h
    src/p6vm.h
    src/p6vxcommon.h
    src/pc6001v.h
    src/pio.h
    src/psgfm.h
    src/replay.h
    src/romaji.h
    src/schedule.h
    src/sound.h
    src/status.h
    src/tape.h
    src/typedef.h
    src/vdg.h
    src/voice.h
    src/vsurface.h
)

set(PC6001VX_FORMS
    src/Qt/configdialog.ui
    src/Qt/systeminfodialog.ui
    src/Qt/virtualkeytabwidget.ui
)

set(PC6001VX_RESOURCES
    src/Qt/pc6001vx.qrc
)

qt_add_executable(PC6001VX MANUAL_FINALIZATION WIN32
    ${PC6001VX_SOURCES}
    ${PC6001VX_HEADERS}
    ${PC6001VX_FORMS}
    ${PC6001VX_RESOURCES}
)

target_include_directories(PC6001VX PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Qt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Qt/babel
)

target_compile_definitions(PC6001VX PRIVATE
    QTP6VX
    USEFILESYSTEM
)

if(ANDROID)
    target_compile_definitions(PC6001VX PRIVATE
        NOLIBINTL
        NOSINGLEAPP
        NOMONITOR
        NOAVI
        NO_HWACCEL
        ALWAYSFULLSCREEN
        AUTOSUSPEND
        CUSTOMROMPATH=\"\"
    )

    set_target_properties(PC6001VX PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(PC6001VX PRIVATE DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR NOT CMAKE_BUILD_TYPE)
    target_compile_definitions(PC6001VX PRIVATE QT_NO_DEBUG_OUTPUT)
endif()

if(NOT WIN32)
    target_compile_definitions(PC6001VX PRIVATE __stdcall=)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(PC6001VX PRIVATE -Wno-unused-parameter -Wno-unused-variable)
    if(ANDROID)
        target_compile_options(PC6001VX PRIVATE -Wno-error=format-security -Wno-format-security)
    endif()
endif()

if(ANDROID)
    target_compile_definitions(PC6001VX PRIVATE NOJOYSTICK NOAVI)
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(X11 QUIET x11)
        if(X11_FOUND)
            target_compile_definitions(PC6001VX PRIVATE USE_X11)
            target_include_directories(PC6001VX PRIVATE ${X11_INCLUDE_DIRS})
            target_link_libraries(PC6001VX PRIVATE ${X11_LIBRARIES})
        endif()

        pkg_check_modules(SDL2 QUIET sdl2)
        if(SDL2_FOUND)
            target_include_directories(PC6001VX PRIVATE ${SDL2_INCLUDE_DIRS})
            target_link_libraries(PC6001VX PRIVATE ${SDL2_LIBRARIES})
        else()
            target_compile_definitions(PC6001VX PRIVATE NOJOYSTICK)
        endif()

        pkg_check_modules(LIBAVFORMAT QUIET libavformat)
        pkg_check_modules(LIBAVCODEC QUIET libavcodec)
        pkg_check_modules(LIBSWSCALE QUIET libswscale)
        pkg_check_modules(LIBSWRESAMPLE QUIET libswresample)
        pkg_check_modules(LIBAVUTIL QUIET libavutil)
        if(LIBAVFORMAT_FOUND AND LIBAVCODEC_FOUND AND LIBSWSCALE_FOUND AND LIBSWRESAMPLE_FOUND AND LIBAVUTIL_FOUND)
            target_compile_definitions(PC6001VX PRIVATE __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS)
            target_include_directories(PC6001VX PRIVATE
                ${LIBAVFORMAT_INCLUDE_DIRS}
                ${LIBAVCODEC_INCLUDE_DIRS}
                ${LIBSWSCALE_INCLUDE_DIRS}
                ${LIBSWRESAMPLE_INCLUDE_DIRS}
                ${LIBAVUTIL_INCLUDE_DIRS}
            )
            target_link_libraries(PC6001VX PRIVATE
                ${LIBAVFORMAT_LIBRARIES}
                ${LIBAVCODEC_LIBRARIES}
                ${LIBSWSCALE_LIBRARIES}
                ${LIBSWRESAMPLE_LIBRARIES}
                ${LIBAVUTIL_LIBRARIES}
            )
        else()
            target_compile_definitions(PC6001VX PRIVATE NOAVI)
        endif()
    else()
        target_compile_definitions(PC6001VX PRIVATE NOJOYSTICK NOAVI)
    endif()
endif()

set_target_properties(PC6001VX PROPERTIES
    OUTPUT_NAME PC6001VX
    VERSION ${PROJECT_VERSION}
    QT_TARGET_VERSION "${PROJECT_VERSION}"
    QT_TARGET_COMPANY_NAME "eighttails"
    QT_TARGET_DESCRIPTION "PC6001VX"
    QT_TARGET_COPYRIGHT "eighttails"
    QT_TARGET_PRODUCT_NAME "PC6001VX"
)

target_link_libraries(PC6001VX PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    SingleApplication::SingleApplication
)

if(WIN32)
    set_target_properties(PC6001VX PROPERTIES
        QT_TARGET_RC_ICONS "${CMAKE_CURRENT_SOURCE_DIR}/src/win32/PC6001VX.ico"
    )

    qt_import_plugins(PC6001VX
        NO_DEFAULT
        INCLUDE_BY_TYPE
            platforms Qt::QWindowsIntegrationPlugin
            styles Qt::QModernWindowsStylePlugin
            networkinformation Qt::QNLMNIPlugin
            tls Qt::QSchannelBackendPlugin Qt::QTlsBackendOpenSSLPlugin
            multimedia Qt::QWindowsMediaPlugin
            imageformats Qt::QGifPlugin Qt::QICNSPlugin Qt::QICOPlugin Qt::QJpegPlugin Qt::QSvgPlugin Qt::QTgaPlugin Qt::QTiffPlugin Qt::QWbmpPlugin Qt::QWebpPlugin
            iconengines Qt::QSvgIconPlugin
    )

    _qt_internal_generate_win32_rc_file(PC6001VX)

    target_link_options(PC6001VX PRIVATE -static)
    target_link_options(PC6001VX PRIVATE -static-libgcc -static-libstdc++)

    target_link_libraries(PC6001VX PRIVATE
        "-Wl,-Bstatic"
        "${PC6001VX_STATIC_STDCXX}"
        "${PC6001VX_STATIC_GCC_EH}"
        "${PC6001VX_STATIC_GCC}"
        "-Wl,-Bdynamic"
    )

    target_link_libraries(PC6001VX PRIVATE
        "-Wl,-Bstatic"
        winpthread
        pthread
        SDL2
        ogg
        vorbis
        vorbisenc
        vorbisfile
        intl
        iconv
        crypt32
        ws2_32
        gdi32
        advapi32
        user32
        shell32
        "-Wl,-Bdynamic"
    )
endif()

if(TARGET Qt6::lrelease)
    qt_add_translations(PC6001VX
        TS_FILES
            src/Qt/translation/PC6001VX_en.ts
            src/Qt/translation/PC6001VX_ru.ts
    )
endif()

# babel.cpp is #include-d directly by osdQt.cpp; exclude it from compilation
set_source_files_properties(src/Qt/babel/babel.cpp PROPERTIES HEADER_FILE_ONLY TRUE)

qt_finalize_executable(PC6001VX)

# Source groups for Qt Creator project tree
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${PC6001VX_SOURCES}
    ${PC6001VX_HEADERS}
    ${PC6001VX_FORMS}
    ${PC6001VX_RESOURCES}
)
