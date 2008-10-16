TEMPLATE = app
DEPENDPATH += . forms src
INCLUDEPATH += . src
QT += network gui core
CONFIG += warn_on thread qt resources release
RESOURCES += resource.qrc

UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp

win32 {
 RC_FILE = win/icon.rc
}

unix {
 INSTALL_PREFIX =  /usr
 target.path =  $${INSTALL_PREFIX}/bin
 INSTALLS =  target \
}

macx {
 CONFIG +=  x86 ppc
 ICON =  macx/icon.icns
}

HEADERS += src/about.h src/mainform.h src/manager.h src/threads.h
FORMS += forms/about.ui forms/mainform.ui forms/wiimanager.ui
SOURCES += src/main.cpp src/about.cpp src/mainform.cpp src/manager.cpp src/networkthread.cpp src/streamthread.cpp

