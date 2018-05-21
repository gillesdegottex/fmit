# Copyright 2015 "Gilles Degottex"

# This file is part of "FMIT"

# "FMIT" is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.

# "FMIT" is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


# Audio Capture Systems: acs_qt, acs_alsa, acs_jack, acs_portaudio, acs_oss
#                        (only acs_qt works on Windows)
CONFIG += acs_qt


# ------------------------------------------------------------------------------
# (modify the following at your own risks !) -----------------------------------

message(CONFIG=$$CONFIG)

# Retrieve the version number, first from git
# (if fail, fall back on the version present in the README.txt file)
FMITVERSIONPRO = $$system(git describe --tags --always)
isEmpty(FMITVERSIONPRO){
    FMITVERSIONPRO = $$system(cat README.txt |sed -n '3p' |sed 's/Version\ //')
}
FMITBRANCHGITPRO = $$system(git rev-parse --abbrev-ref HEAD)
message(Git: FMIT version: $$FMITVERSIONPRO Branch: $$FMITBRANCHGITPRO)
DEFINES += FMITVERSION=$$FMITVERSIONPRO
DEFINES += FMITBRANCHGIT=$$FMITBRANCHGITPRO

# To place the application's files in the proper folder
isEmpty(PREFIX){
    PREFIX = /usr/local
}
unix:DEFINES += PREFIX=$$PREFIX
# To place the shortcut in the proper folder
isEmpty(PREFIXSHORTCUT){
    PREFIXSHORTCUT = /usr
}

# Manage Architectures
unix:message(For Linux)
win32:message(For Windows)
msvc:message(Using MSVC compiler)
gcc:message(Using GCC compiler)
contains(QT_ARCH, i386):message(For 32bits)
contains(QT_ARCH, x86_64):message(For 64bits)
unix:message(Installation path: $$PREFIX)

# ------------------------------------------------------------------------------
# Manage options for Audio Capture System --------------------------------------

CONFIG(acs_qt) {
    message(Audio Capture System: Request Qt support)
    DEFINES += CAPTURE_QT
    QT += multimedia
    SOURCES += src/CaptureThreadImplQt.cpp
}

CONFIG(acs_alsa) {
    message(Audio Capture System: Request ALSA support)
    DEFINES += CAPTURE_ALSA
    QT += multimedia
    SOURCES += src/CaptureThreadImplALSA.cpp
    LIBS += -lasound
}

CONFIG(acs_jack) {
    message(Audio Capture System: Request JACK support)
    SOURCES += src/CaptureThreadImplJACK.cpp
    DEFINES += CAPTURE_JACK
    LIBS += -ljack
}

CONFIG(acs_portaudio) {
    message(Audio Capture System: Request PortAudio support)
    DEFINES += CAPTURE_PORTAUDIO
    SOURCES += src/CaptureThreadImplPortAudio.cpp
    LIBS += -lportaudio
}

CONFIG(acs_oss) {
    message(Audio Capture System: Request OSS support)
    DEFINES += CAPTURE_OSS
    SOURCES += src/CaptureThreadImplOSS.cpp
    LIBS += -lasound
}

# FFTW -------------------------------------------------------------------------
win32 {
    isEmpty(FFT_LIBDIR) {
        contains(QMAKE_TARGET.arch, x86_64) {
            FFT_LIBDIR = "$$_PRO_FILE_PWD_/../lib/fftw-3.3.4-dll64"
        } else {
            FFT_LIBDIR = "$$_PRO_FILE_PWD_/../lib/fftw-3.3.4-dll32"
        }
    }
    !isEmpty(FFT_LIBDIR){
        message(FFT_LIBDIR=$$FFT_LIBDIR)
        INCLUDEPATH += $$FFT_LIBDIR
        LIBS += -L$$FFT_LIBDIR
    }
    msvc: LIBS += $$FFT_LIBDIR/libfftw3-3.lib
    gcc: LIBS += -lfftw3-3
    # msvc: LIBS += $$FFT_LIBDIR/libfftw3f-3.lib
    # gcc: LIBS += -lfftw3f-3
}
unix {
    !isEmpty(FFT_LIBDIR){
        message(FFT_LIBDIR=$$FFT_LIBDIR)
        INCLUDEPATH += $$FFT_LIBDIR/include
        LIBS += -L$$FFT_LIBDIR/lib
    }
    LIBS += -lfftw3
    # LIBS += -lfftw3f
}

# Common configurations --------------------------------------------------------

win32 {
    msvc: LIBS += opengl32.lib
    msvc: LIBS += glu32.lib
    gcc: LIBS += -lopengl32
    gcc: LIBS += -glu32
}


QT += core gui opengl multimedia svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
macx: QT += network

INCLUDEPATH += libs

TARGET = fmit
TEMPLATE = app

RC_ICONS = ui/images/fmit.ico

SOURCES +=  src/main.cpp \
            src/CustomInstrumentTunerForm.cpp \
            src/aboutbox.cpp \
            src/AutoQSettings.cpp \
            src/CaptureThread.cpp \
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
            libs/CppAddons/CAMath.cpp


HEADERS  += src/CustomInstrumentTunerForm.h \
            src/aboutbox.h \
            src/AutoQSettings.h \
            src/CaptureThread.h \
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
            libs/CppAddons/StringAddons.h

FORMS    += ui/InstrumentTunerForm.ui \
            ui/ConfigForm.ui \
            ui/aboutbox.ui

RESOURCES += fmit.qrc

TRANSLATIONS = tr/fmit_de.ts \
               tr/fmit_el.ts \
               tr/fmit_en.ts \
               tr/fmit_fr.ts \
               tr/fmit_pt.ts \
               tr/fmit_pt_BR.ts \
               tr/fmit_ru.ts


CONFIG += embed_manifest_exe

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
message(QMAKE_LRELEASE=$$QMAKE_LRELEASE)
lrelease.commands = $$QMAKE_LRELEASE $$_PRO_FILE_PWD_/fmit.pro
QMAKE_EXTRA_TARGETS += lrelease

# Installation configurations --------------------------------------------------
scales.path = $$PREFIX/share/fmit/scales
scales.files = scales/*
translations.path = $$PREFIX/share/fmit/tr
translations.files = tr/*.qm
target.path = $$PREFIX/bin
shortcut.path = $$PREFIXSHORTCUT/share/applications
shortcut.files = distrib/fmit.desktop
iconsvg.path = $$PREFIX/share/icons/hicolor/scalable/apps
iconsvg.files = ui/images/fmit.svg
iconpng.path = $$PREFIX/share/icons/hicolor/128x128/apps
iconpng.files = ui/images/fmit.png
appdata.path = $$PREFIX/share/appdata
appdata.files = distrib/fmit.appdata.xml
INSTALLS += target scales translations shortcut iconsvg iconpng appdata
