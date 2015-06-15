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


// #include "config.h"

#include <stdlib.h>
#include <iostream>
using namespace std;
#include <signal.h>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <GL/glut.h>

#include "CppAddons/CAMath.h"

#include "CustomInstrumentTunerForm.h"
CustomInstrumentTunerForm* g_main_form = NULL;

void signal_interrupt(int sig)
{
	cerr << "Interrupt" << endl;

	delete g_main_form;

	exit(sig);
}
void signal_term(int sig)
{
	cerr << "No No ! Pleeeeeease ... *argl*" << endl;
	cerr << endl;
    cerr << PACKAGE_NAME << " nastily killed with signal " << sig << endl;

	exit(sig);
}

int main(int argc, char** argv)
{
    cerr << "Free Music Instrument Tuner version " << PACKAGE_VERSION << endl;
	cerr << "Install directory '" << PREFIX << "'" << endl;

	signal(SIGINT, signal_interrupt);
	signal(SIGTERM, signal_term);

	QApplication a(argc, argv, true);

	glutInit(&argc, argv);

//	if(a.argc()>1)
//	{
//		if(a.argc()>2 && (QString(a.argv()[1])=="-d" || QString(a.argv()[1])=="--device"))
//		{
//			cerr << "used device: " << a.argv()[2] << endl;
//		}
//	}

//	cerr << "LANG=" << QTextCodec::locale() << endl;

	QTranslator tr_fmit;
	QString trFile = QString("fmit_") + QLocale::system().name();
	QString trPath = QString(PREFIX) + QString("/share/fmit/tr");
	cerr << "Looking up translation '" << trFile.toStdString() << "' in '" << trPath.toStdString() << "'" << endl;
	tr_fmit.load(trFile, trPath);
	a.installTranslator(&tr_fmit);

	g_main_form = new CustomInstrumentTunerForm();

	g_main_form->show();

	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	a.exec();

	delete g_main_form;

	return 0;
}

