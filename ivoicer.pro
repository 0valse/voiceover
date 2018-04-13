TEMPLATE = app

QT += widgets network multimedia

FORMS += ui/mainform.ui
RESOURCES += audio/music.qrc data/icons.qrc
SOURCES += main.cpp mainform.cpp
SOURCES += multidownloader.cpp
HEADERS += mainform.h
HEADERS += multidownloader.h
LIBS += -L/usr/lib -luchardet


MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build

DESTDIR = build
