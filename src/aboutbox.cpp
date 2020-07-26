// Copyright (C) 2014  Gilles Degottex <gilles.degottex@gmail.com>

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


#include "aboutbox.h"
#include "ui_aboutbox.h"

#include <QFile>
#include <QTextStream>

//#include "wmainwindow.h"
#include "qthelper.h"
//#include "qaesigproc.h"
//#include "ftsound.h"

extern QString FMITVersion();

AboutBox::AboutBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutBox)
{
    ui->setupUi(this);

    QString fullversion = "Version "+FMITVersion();
    fullversion += "\nCompiled by "+getCompilerVersion()+" for ";
    #ifdef Q_PROCESSOR_X86_32
      fullversion += "32bits";
    #endif
    #ifdef Q_PROCESSOR_X86_64
      fullversion += "64bits";
    #endif
    fullversion += "\nCompiled with Qt "+QString(QT_VERSION_STR)+" Running with Qt "+qVersion(); // Add Qt versions
    ui->lblVersion->setText(fullversion);

    QString txt = "";

    txt += "<h4>Purpose</h4>";
    txt += "<i>FMIT</i> is a graphical utility for tuning your musical instruments, with error and volume history and advanced features. ";

    txt += "<p>To suggest a new functionality or report a bug, do not hesitate to <a href='https://github.com/gillesdegottex/fmit/issues'>raise an issue on GitHub.</a></p>";

    txt += "<h4>Legal</h4>\
            The core of this software is under the <a href=\"http://www.gnu.org/licenses/gpl.html\">GPL License</a>.\
            All original source files of any kind (code source and any ressources) are under the same license, except\
            the content of the \"libs\" directory, which contains imported source code under various licenses.\
            <br/><br/>\
            Copyright &copy; 2004 Gilles Degottex <a href='mailto:gilles.degottex@gmail.com'>&lt;gilles.degottex@gmail.com&gt;</a><br/>\
            <i>FMIT</i> is coded in C++/<a href='http://qt-project.org'>Qt</a>.\
            The source code is hosted on <a href='https://github.com/gillesdegottex/fmit'>GitHub</a>.";

    txt += "<h4>Disclaimer</h4>\
            ALL THE FUNCTIONALITIES OF <I>FMIT</I> AND ITS CODE ARE PROVIDED WITHOUT ANY WARRANTY \
            (E.G. THERE IS NO WARRANTY OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE). \
            ALSO, THE COPYRIGHT HOLDERS AND CONTRIBUTORS DO NOT TAKE ANY LEGAL RESPONSIBILITY \
            REGARDING THE IMPLEMENTATIONS OF THE PROCESSING TECHNIQUES OR ALGORITHMS \
            (E.G. CONSEQUENCES OF BUGS OR ERRONEOUS IMPLEMENTATIONS).<br/> \
            Please see the README.txt file for additional information.";

    ui->txtAbout->setText(txt);


    // Libraries

    // FFT
//    QString fftinfostr = "";
//    fftinfostr += "<i>Fast Fourier Transform (FFT):</i> "+qae::FFTwrapper::getLibraryInfo();
//    fftinfostr += " ("+QString::number(sizeof(FFTTYPE)*8)+"bits(";
//    if(sizeof(FFTTYPE)==4)  fftinfostr += "single";
//    if(sizeof(FFTTYPE)==8)  fftinfostr += "double";
//    if(sizeof(FFTTYPE)==16)  fftinfostr += "quadruple";
//    fftinfostr += "); smallest: "+QString::number(20*log10(std::numeric_limits<FFTTYPE>::min()))+"dB)";
//    ui->vlLibraries->addWidget(new QLabel(fftinfostr, this));

    //    QMessageBox::aboutQt(this, "About this software");
}

AboutBox::~AboutBox()
{
    delete ui;
}
