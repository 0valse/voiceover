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

CXX_FILES_TO_BUILD = CharDistribution.cpp JpCntx.cpp LangModels/LangArabicModel.cpp LangModels/LangBulgarianModel.cpp LangModels/LangRussianModel.cpp LangModels/LangEsperantoModel.cpp LangModels/LangFrenchModel.cpp LangModels/LangDanishModel.cpp LangModels/LangGermanModel.cpp LangModels/LangGreekModel.cpp LangModels/LangHungarianModel.cpp LangModels/LangHebrewModel.cpp LangModels/LangSpanishModel.cpp LangModels/LangThaiModel.cpp LangModels/LangTurkishModel.cpp LangModels/LangVietnameseModel.cpp nsHebrewProber.cpp nsCharSetProber.cpp nsBig5Prober.cpp nsEUCJPProber.cpp nsEUCKRProber.cpp nsEUCTWProber.cpp nsEscCharsetProber.cpp nsEscSM.cpp nsGB2312Prober.cpp nsMBCSGroupProber.cpp nsMBCSSM.cpp nsSBCSGroupProber.cpp nsSBCharSetProber.cpp nsSJISProber.cpp nsUTF8Prober.cpp nsLatin1Prober.cpp nsUniversalDetector.cpp uchardet.cpp

UCHARDET_BUILD_PATH = $$PWD/build/uchardet
UCHARDET_PATH = $$PWD/uchardet-0.0.6/src

defineTest(build_cxx) {
    system(mkdir -p $$PWD/build/uchardet/LangModels 2>/dev/null || /bin/true)
    for (FILENAME, CXX_FILES_TO_BUILD) {
        message(Build $${FILENAME})
        system($$QMAKE_CXX -DVERSION=\"0.0.6\" -pipe -O2 -march=x86-64 -mtune=generic -O2 -pipe -fstack-protector-strong -fno-plt -Wall -W -D_REENTRANT -fPIC -I$$UCHARDET_PATH -I$$UCHARDET_PATH/LangModels -I$$UCHARDET_PATH/tools  -I/usr/lib/qt/mkspecs/linux-g++ -c $$UCHARDET_PATH/$${FILENAME} -o $$UCHARDET_BUILD_PATH/$${FILENAME}.o)
        QMAKE_POST_OBJ += $$UCHARDET_BUILD_PATH/$${FILENAME}.o
        message($$UCHARDET_BUILD_PATH/$${FILENAME}.o)
    }
    system(ar qc $$UCHARDET_BUILD_PATH/libuchardet.a $$QMAKE_POST_OBJ)
    message(ar libuchardet.a)
    system(ranlib $$UCHARDET_BUILD_PATH/libuchardet.a)
    message(ranlib libuchardet.a)
}
        
build_cxx()

LIBS += $$UCHARDET_BUILD_PATH/libuchardet.a
INCLUDEPATH += $$UCHARDET_PATH

MOC_DIR = build/moc
OBJECTS_DIR = build/src
UI_DIR = build/ui
RCC_DIR = build/rcc

DESTDIR = build
