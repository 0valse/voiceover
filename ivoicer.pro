TEMPLATE = app

QT += widgets network multimedia

FORMS += ui/mainform.ui \
         ui/aboutform.ui \
         ui/settingsform.ui
RESOURCES += data/audio/music.qrc \
             data/icons/icons.qrc


UCHARDET_PATH = $$PWD/uchardet-0.0.6/src
             
CXX_FILES_TO_BUILD = $$UCHARDET_PATH/CharDistribution.cpp $$UCHARDET_PATH/JpCntx.cpp $$UCHARDET_PATH/LangModels/LangArabicModel.cpp $$UCHARDET_PATH/LangModels/LangBulgarianModel.cpp $$UCHARDET_PATH/LangModels/LangRussianModel.cpp $$UCHARDET_PATH/LangModels/LangEsperantoModel.cpp $$UCHARDET_PATH/LangModels/LangFrenchModel.cpp $$UCHARDET_PATH/LangModels/LangDanishModel.cpp $$UCHARDET_PATH/LangModels/LangGermanModel.cpp $$UCHARDET_PATH/LangModels/LangGreekModel.cpp $$UCHARDET_PATH/LangModels/LangHungarianModel.cpp $$UCHARDET_PATH/LangModels/LangHebrewModel.cpp $$UCHARDET_PATH/LangModels/LangSpanishModel.cpp $$UCHARDET_PATH/LangModels/LangThaiModel.cpp $$UCHARDET_PATH/LangModels/LangTurkishModel.cpp $$UCHARDET_PATH/LangModels/LangVietnameseModel.cpp $$UCHARDET_PATH/nsHebrewProber.cpp $$UCHARDET_PATH/nsCharSetProber.cpp $$UCHARDET_PATH/nsBig5Prober.cpp $$UCHARDET_PATH/nsEUCJPProber.cpp $$UCHARDET_PATH/nsEUCKRProber.cpp $$UCHARDET_PATH/nsEUCTWProber.cpp $$UCHARDET_PATH/nsEscCharsetProber.cpp $$UCHARDET_PATH/nsEscSM.cpp $$UCHARDET_PATH/nsGB2312Prober.cpp $$UCHARDET_PATH/nsMBCSGroupProber.cpp $$UCHARDET_PATH/nsMBCSSM.cpp $$UCHARDET_PATH/nsSBCSGroupProber.cpp $$UCHARDET_PATH/nsSBCharSetProber.cpp $$UCHARDET_PATH/nsSJISProber.cpp $$UCHARDET_PATH/nsUTF8Prober.cpp $$UCHARDET_PATH/nsLatin1Prober.cpp $$UCHARDET_PATH/nsUniversalDetector.cpp $$UCHARDET_PATH/uchardet.cpp

HEADERS_TO_BUILD = $$UCHARDET_PATH/CharDistribution.h $$UCHARDET_PATH/JpCntx.h $$UCHARDET_PATH/nsBig5Prober.h $$UCHARDET_PATH/nsCharSetProber.h $$UCHARDET_PATH/nsCodingStateMachine.h $$UCHARDET_PATH/nscore.h $$UCHARDET_PATH/nsEscCharsetProber.h $$UCHARDET_PATH/nsEUCJPProber.h $$UCHARDET_PATH/nsEUCKRProber.h $$UCHARDET_PATH/nsEUCTWProber.h $$UCHARDET_PATH/nsGB2312Prober.h $$UCHARDET_PATH/nsHebrewProber.h $$UCHARDET_PATH/nsLatin1Prober.h $$UCHARDET_PATH/nsMBCSGroupProber.h $$UCHARDET_PATH/nsPkgInt.h $$UCHARDET_PATH/nsSBCharSetProber.h $$UCHARDET_PATH/nsSBCSGroupProber.h $$UCHARDET_PATH/nsSJISProber.h $$UCHARDET_PATH/nsUniversalDetector.h $$UCHARDET_PATH/nsUTF8Prober.h $$UCHARDET_PATH/prmem.h $$UCHARDET_PATH/uchardet.h
             
SOURCES += \
           src/main.cpp \
           src/mainform.cpp \
           src/multidownloader.cpp \
           src/aboutform.cpp \
           src/settingsform.cpp \
           src/settings.cpp \
           $$CXX_FILES_TO_BUILD
HEADERS +=  \
           src/mainform.h \
           src/multidownloader.h \
           src/aboutform.h \
           src/settingsform.h \
           src/settings.h \
           $$HEADERS_TO_BUILD


INCLUDEPATH += $$UCHARDET_PATH

MOC_DIR = build/moc
OBJECTS_DIR = build/src
UI_DIR = build/ui
RCC_DIR = build/rcc

DESTDIR = build
