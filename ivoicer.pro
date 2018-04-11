TEMPLATE = app

QT += widgets network

FORMS += ui/mainform.ui
SOURCES += main.cpp mainform.cpp
SOURCES += multidownloader.cpp
HEADERS += mainform.h
HEADERS += multidownloader.h
LIBS += -L/usr/lib -luchardet


MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build

DESTDIR = build
