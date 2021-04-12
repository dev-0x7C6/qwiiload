# -------------------------------------------------
# Project created by QtCreator 2009-03-20T00:00:12
# -------------------------------------------------
QT += network gui widgets
QMAKE_CXXFLAGS += -std=c++20
TARGET = qwiiload
TEMPLATE = app
SOURCES += src/main.cpp \
    src/dialogs.cpp \
    src/mainwindow.cpp \
    src/about.cpp \
    src/uploadthread.cpp
HEADERS += src/mainwindow.h \
    src/dialogs.hpp \
    src/about.h \
    src/uploadthread.h
FORMS += ui/mainwindow.ui \
    ui/about.ui
RESOURCES += resource.qrc

UI_DIR = src
MOC_DIR = src
RCC_DIR = src
OBJECTS_DIR = src

win32 {
 RC_FILE = win/icon.rc
}

unix {
 INSTALL_PREFIX =  /usr
 target.path =  $${INSTALL_PREFIX}/bin
 icons.path =  $${INSTALL_PREFIX}/share/icons
 icons.files =  images/$${TARGET}.png  images/$${TARGET}48.png  images/$${TARGET}128.png
 doc.path = $${INSTALL_PREFIX}/share/doc/qwiiload
 doc.files = doc/CHANGELOG doc/COPYING doc/README
 desktop.path =  $${INSTALL_PREFIX}/share/applications
 desktop.files =  res/$${TARGET}.desktop
 dolphin_integration.path =  $${INSTALL_PREFIX}/share/apps/dolphin/servicemenus
 dolphin_integration.files =  res/$${TARGET}-send.desktop
 d3lphin_integration.path =  $${INSTALL_PREFIX}/share/apps/d3lphin/servicemenus
 d3lphin_integration.files =  res/$${TARGET}-send.desktop
 konqueror_integration.path =  $${INSTALL_PREFIX}/share/apps/konqueror/servicemenus
 konqueror_integration.files =  res/$${TARGET}-send.desktop
 kde4_integration.path =  $${INSTALL_PREFIX}/lib/kde4/share/kde4/services/ServiceMenus
 kde4_integration.files = res/$${TARGET}-send.desktop
 INSTALLS =  target \
  doc \
  icons \
  desktop \
  dolphin_integration \
  d3lphin_integration \
  konqueror_integration \
  kde4_integration
}

macx {
 QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk
 CONFIG-=app_bundle
 CONFIG+=x86 ppc
 ICON =  macx/icon.icns
}
