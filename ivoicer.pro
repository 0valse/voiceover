TEMPLATE = app

QT += widgets network multimedia

FORMS += ui/mainform.ui \
         ui/aboutform.ui \
         ui/settingsform.ui
RESOURCES += data/audio/music.qrc \
             data/icons/icons.qrc

SOURCES += src/main.cpp \
           src/mainform.cpp \
           src/multidownloader.cpp \
           src/aboutform.cpp \
           src/settingsform.cpp \
           src/settings.cpp
HEADERS += src/mainform.h \
           src/multidownloader.h \
           src/aboutform.h \
           src/settingsform.h \
           src/settings.h

#uchardetDep.target = uchardet-0.0.6/src/libuchardet.a
uchardetDep.commands = mkdir $$PWD/build/uchardet ; cd $$PWD/build/uchardet && cmake $$PWD/uchardet-0.0.6 && make clean && make libuchardet_static && cd $$PWD/build
QMAKE_EXTRA_TARGETS += uchardetDep
PRE_TARGETDEPS += uchardetDep

LIBS += $$PWD/build/uchardet/src/libuchardet.a
INCLUDEPATH += $$PWD/uchardet-0.0.6/src

MOC_DIR = build/moc
OBJECTS_DIR = build/src
UI_DIR = build/ui
RCC_DIR = build/rcc

DESTDIR = build
