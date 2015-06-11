



# TODO
DEFINES += "PACKAGE_NAME=\\\"FMIT\\\""
DEFINES += "PACKAGE_VERSION=\\\"1.99.0\\\""
DEFINES += "PREFIX=\\\"/usr/local/\\\""


# ------------------------------------------------

DEFINES += CAPTURE_ALSA

message($$DEFINES)


# ------------------------------------------------------------------------------
# (modify the following at your own risks !) -----------------------------------

QT       += core gui opengl

LIBS += -lglut -lGLU -lGL -lfftw3 -lasound

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += libs

TARGET = fmit
TEMPLATE = app


SOURCES +=  src/main.cpp \
            src/CustomInstrumentTunerForm.cpp \
            src/AutoQSettings.cpp \
            src/CaptureThread.cpp \
            src/CaptureThreadImplALSA.cpp \
#            src/CaptureThreadImplJACK.cpp \
#            src/CaptureThreadImplOSS.cpp \
#            src/CaptureThreadImplPortAudio.cpp \
            src/DummyMonoQuantizer.cpp \
            src/LatencyMonoQuantizer.cpp \
            src/MonoQuantizer.cpp \
            src/modules/View.cpp \
            src/modules/DialView.cpp \
            src/modules/GLErrorHistory.cpp \
            src/modules/GLFreqStruct.cpp \
            src/modules/GLFT.cpp \
            src/modules/GLGraph.cpp \
            src/modules/GLSample.cpp \
            src/modules/GLStatistics.cpp \
            src/modules/GLVolumeHistory.cpp \
            src/modules/MicrotonalView.cpp \
            libs/Music/Algorithm.cpp \
            libs/Music/Autocorrelation.cpp \
            libs/Music/CFFTW3.cpp \
            libs/Music/CombedFT.cpp \
            libs/Music/Convolution.cpp \
            libs/Music/CumulativeDiff.cpp \
            libs/Music/CumulativeDiffAlgo.cpp \
            libs/Music/Filter.cpp \
            libs/Music/FreqAnalysis.cpp \
            libs/Music/LPC.cpp \
            libs/Music/MultiCumulativeDiffAlgo.cpp \
            libs/Music/Music.cpp \
            libs/Music/Note.cpp \
            libs/Music/SPWindow.cpp \
            libs/Music/TimeAnalysis.cpp \
            libs/CppAddons/CAMath.cpp \
            libs/CppAddons/Random.cpp


HEADERS  += src/CustomInstrumentTunerForm.h \
            src/AutoQSettings.h \
            src/CaptureThread.h \
            src/CaptureThreadImplALSA.h \
#            src/CaptureThreadImplJACK.h \
            src/DummyMonoQuantizer.h \
            src/LatencyMonoQuantizer.h \
            src/MonoQuantizer.h \
            src/modules/View.h \
            src/modules/DialView.h \
            src/modules/GLErrorHistory.h \
            src/modules/GLFreqStruct.h \
            src/modules/GLFT.h \
            src/modules/GLGraph.h \
            src/modules/GLSample.h \
            src/modules/GLStatistics.h \
            src/modules/GLVolumeHistory.h \
            src/modules/MicrotonalView.h \
            libs/Music/Algorithm.h \
            libs/Music/Autocorrelation.h \
            libs/Music/CFFTW3.h \
            libs/Music/CombedFT.h \
            libs/Music/Convolution.h \
            libs/Music/CumulativeDiff.h \
            libs/Music/CumulativeDiffAlgo.h \
            libs/Music/Filter.h \
            libs/Music/FreqAnalysis.h \
            libs/Music/LPC.h \
            libs/Music/MultiCumulativeDiffAlgo.h \
            libs/Music/Music.h \
            libs/Music/Note.h \
            libs/Music/SPWindow.h \
            libs/Music/TimeAnalysis.h \
            libs/CppAddons/CAMath.h \
            libs/CppAddons/Fit.h \
            libs/CppAddons/Observer.h \
            libs/CppAddons/Random.h \
            libs/CppAddons/Singleton.h \
            libs/CppAddons/StringAddons.h

FORMS    += ui/InstrumentTunerForm.ui \
            ui/ConfigForm.ui

RESOURCES += ui/fmit.qrc
