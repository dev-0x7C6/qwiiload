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
 INSTALLS =  target \
}

macx {
 QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk
 CONFIG-=app_bundle
 CONFIG+=x86 ppc
 ICON =  macx/icon.icns
}

HEADERS += src/about.h src/mainform.h src/manager.h src/threads.h
FORMS += forms/about.ui forms/mainform.ui forms/wiimanager.ui
SOURCES += src/main.cpp src/about.cpp src/mainform.cpp src/manager.cpp src/networkthread.cpp src/streamthread.cpp

