TEMPLATE = app
DEPENDPATH += . forms src
INCLUDEPATH += . src
QT += network gui core
CONFIG += warn_on thread qt resources release
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
 icons.path =  /usr/share/icons
 icons.files =  icons/$${TARGET}.png  icons/$${TARGET}48.png  icons/$${TARGET}128.png 
 doc.path = /usr/share/doc/wiitcpload
 doc.files = doc/*.*
 desktop.path =  /usr/share/applications
 desktop.files =  res/$${TARGET}.desktop
 dolphin_integration.path =  /usr/share/apps/dolphin/servicemenus
 dolphin_integration.files =  res/$${TARGET}-send.desktop
 d3lphin_integration.path =  /usr/share/apps/d3lphin/servicemenus
 d3lphin_integration.files =  res/$${TARGET}-send.desktop
 konqueror_integration.path =  /usr/share/apps/konqueror/servicemenus
 konqueror_integration.files =  res/$${TARGET}-send.desktop
 kde4_integration.path =  /usr/lib/kde4/share/kde4/services/ServiceMenus
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

HEADERS += src/about.h src/mainform.h src/threads.h
FORMS += forms/about.ui forms/mainform.ui
SOURCES += src/main.cpp src/about.cpp src/mainform.cpp src/streamthread.cpp src/networkthread.cpp

