TEMPLATE = app

QT += widgets network multimedia

FORMS += ui/mainform.ui
RESOURCES += data/audio/music.qrc data/icons/icons.qrc

SOURCES += src/main.cpp src/mainform.cpp src/multidownloader.cpp
HEADERS += src/mainform.h src/multidownloader.h

#uchardetDep.target = uchardet-0.0.6/src/libuchardet.a
uchardetDep.commands = cd uchardet-0.0.6 && cmake . && make clean && make libuchardet_static
QMAKE_EXTRA_TARGETS += uchardetDep
PRE_TARGETDEPS += uchardetDep

LIBS += uchardet-0.0.6/src/libuchardet.a

MOC_DIR = build/moc
OBJECTS_DIR = build/src
UI_DIR = build/ui
RCC_DIR = build/rcc

DESTDIR = build
