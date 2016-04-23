// Copyright 2004 "Gilles Degottex"

// This file is part of "fmit"

// "fmit" is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// "fmit" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#include <stdlib.h>
#include <iostream>
#include <cstdlib>
using namespace std;
#include <signal.h>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <QLibraryInfo>

#include "CppAddons/CAMath.h"

#include "qthelper.h"

#include "CustomInstrumentTunerForm.h"
CustomInstrumentTunerForm* g_main_form = NULL;

int main(int argc, char** argv)
{
    QString fmitversiongit(STR(FMITVERSIONGIT));
    QString fmitprefix(STR(PREFIX));

    cout << "Free Music Instrument Tuner (Version " << fmitversiongit.toLatin1().constData() << ")" << endl;

	QApplication a(argc, argv, true);

    // Load translation
    QTranslator qtTranslator;
    QString trFile = "qt_" + QLocale::system().name();
    cout << "INFO: Loading Qt translation file: " << trFile.toLatin1().constData() << " in " << QLibraryInfo::location(QLibraryInfo::TranslationsPath).toLatin1().constData() << endl;
    qtTranslator.load(trFile, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);
    QTranslator fmitTranslator;
    trFile = QString("fmit_")+QLocale::system().name();

    #ifdef Q_OS_WIN32
        QString trPath = QCoreApplication::applicationDirPath()+"/";
    #else
        QString trPath = fmitprefix + QString("/share/fmit/tr");
    #endif

    cout << "INFO: Loading FMIT translation file: " << trFile.toLatin1().constData() << " in " << trPath.toLatin1().constData() << endl;
    fmitTranslator.load(trFile, trPath);
    a.installTranslator(&fmitTranslator);

    g_main_form = new CustomInstrumentTunerForm();

	g_main_form->show();

	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	a.exec();

	delete g_main_form;

	return 0;
}

