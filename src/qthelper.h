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


#ifndef _QAEHELPERS_H_
#define _QAEHELPERS_H_

#include <QRectF>

#include <iostream>

#include <QThread>
#include <QDateTime>
#include <QColor>
#include <QGraphicsView>
#include <QTextStream>
#include <QScrollBar>
#include <QFileInfo>
#include <QAudioFormat>
#include <QDir>
#include <QDebug>

#ifdef __MINGW32__
    #define COMPILER "MinGW"
#elif (defined(__GNUC__) || defined(__GNUG__))
    #define COMPILER "GCC"
    #define COMPILER_VERSION_MAJOR __GNUC__
    #define COMPILER_VERSION_MINOR __GNUC_MINOR__
    #define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
    #define COMPILER "MSVC"
    #define COMPILER_VERSION_MAJOR _MSC_VER/100
    #define COMPILER_VERSION_MINOR _MSC_VER%100
    #if defined(_MSC_FULL_VER)
        #if _MSC_VER >= 1400
            /* _MSC_FULL_VER = VVRRPPPPP */
            #define COMPILER_VERSION_PATCH _MSC_FULL_VER%100000
        #else
            /* _MSC_FULL_VER = VVRRPPPP */
            #define COMPILER_VERSION_PATCH _MSC_FULL_VER%10000
        #endif
    #endif
#endif

inline QString getCompilerVersion(){
    QString version;

    version += QString(COMPILER);
    #ifdef COMPILER_VERSION_MAJOR
        version += " "+QString::number(COMPILER_VERSION_MAJOR);
    #endif
    #ifdef COMPILER_VERSION_MINOR
        version += "."+QString::number(COMPILER_VERSION_MINOR);
    #endif
    #ifdef COMPILER_VERSION_PATCH
        version += "."+QString::number(COMPILER_VERSION_PATCH);
    #endif

    return version;
}


// Check if compiling using MSVC (Microsoft compiler)
#ifdef _MSC_VER
    // The following is necessary for MSVC 2012
    template<class Type> inline Type log2(Type v) {return std::log(v)/std::log(2.0); }
#endif

#define QUOTE(name) #name
#ifdef _MSC_VER
    #define STR(val) QUOTE(val,"")
#else
    #define STR(val) QUOTE(val)
#endif


#define DCOUT std::cout << QThread::currentThreadId() << " " << QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString("hh:mm:ss.zzz             ").toLocal8Bit().constData() << " " << __FILE__ << ":" << __LINE__ << " "

//#define DCOUT QTextStream(stdout) << QThread::currentThreadId() << " " << QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString("hh:mm:ss.zzz             ").toLocal8Bit().constData() << " " << __FILE__ << ":" << __LINE__ << " "

#define DFLAG DCOUT << std::endl;

// Log file and line automatically
#define DLOG qDebug() << __FILE__ << ":" << __LINE__

// Remove hard coded margin (Bug 11945)
// See: https://bugreports.qt-project.org/browse/QTBUG-11945
inline QRectF removeHiddenMargin(QGraphicsView* gv, const QRectF& sceneRect){
    const int bugMargin = 2;
    const double mx = sceneRect.width()/gv->viewport()->size().width()*bugMargin;
    const double my = sceneRect.height()/gv->viewport()->size().height()*bugMargin;
    return sceneRect.adjusted(mx, my, -mx, -my);
}

inline QTextStream& operator<<(QTextStream& stream, const QRectF& rectf) {
    stream << "[" << rectf.left() << "," << rectf.right() << "]x[" << rectf.top() << "," << rectf.bottom() << "]";

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QString& str) {
    stream << str.toLocal8Bit().constData();

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QRectF& rectf) {
    stream << "[" << rectf.left() << "," << rectf.right() << "]x[" << rectf.top() << "," << rectf.bottom() << "]";

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QSize& size) {
    stream << size.width() << "x" << size.height();

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QPoint& p) {
    stream << "(" << p.x() << "," << p.y() << ")";

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QPointF& p) {
    stream << "(" << p.x() << "," << p.y() << ")";

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QColor& c) {
    stream << "(" << c.red() << "," << c.green() << "," << c.blue() << ")";

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const QTime& t) {
    stream << t.toString("hh:mm:ss.zzz").toLocal8Bit().constData();

    return stream;
}

inline QString formatToString(const QAudioFormat &format) {

    QString result;

    if (QAudioFormat() != format) {

        const QString formatEndian = (format.byteOrder() == QAudioFormat::LittleEndian)
            ?   QString("LE") : QString("BE");

        QString formatType;
        switch (format.sampleType()) {
        case QAudioFormat::SignedInt:
            formatType = "signed";
            break;
        case QAudioFormat::UnSignedInt:
            formatType = "unsigned";
            break;
        case QAudioFormat::Float:
            formatType = "float";
            break;
        case QAudioFormat::Unknown:
            formatType = "unknown";
            break;
        }

        QString formatChannels = QString("%1 channels").arg(format.channelCount());
        switch (format.channelCount()) {
        case 1:
            formatChannels = "mono";
            break;
        case 2:
            formatChannels = "stereo";
            break;
        }

        result = format.codec()+" "+QString("%1Hz %2bit %3 %4 %5")
            .arg(format.sampleRate())
            .arg(format.sampleSize())
            .arg(formatType)
            .arg(formatEndian)
            .arg(formatChannels);
    }
    else {
        result = "empty format";
    }

    return result;
}

inline std::ostream& operator<<(std::ostream& stream, const QAudioFormat& af) {
    stream << formatToString(af);
    return stream;
}
inline QTextStream& operator<<(QTextStream& stream, const QAudioFormat& af) {
    stream << formatToString(af);
    return stream;
}

// A scrollbar which reset the mouse cursor when hovered
class QScrollBarHover : public QScrollBar {
public:
    QScrollBarHover(Qt::Orientation orientation, QWidget * parent)
        : QScrollBar(orientation, parent) {
        setMouseTracking(true);
    }

    virtual void mouseMoveEvent(QMouseEvent * e) {
        QScrollBar::mouseMoveEvent(e);
        setCursor(Qt::ArrowCursor);
    }
};

inline QString dropFileExtension(const QString& filepath) {
    QFileInfo fileInfo(filepath);
    return fileInfo.path()+QDir::separator()+fileInfo.completeBaseName();
}

//            COUTD << snd->m_dftparams.nl << " " << snd->m_dftparams.nr << " " << snd->m_dftparams.winlen << " " << snd->m_dftparams.dftlen << " " << snd->m_dftparams.ampscale << " " << snd->m_dftparams.delay << endl;

namespace qae {

inline QString humanReadableSize(float num) {
    static QStringList list;
    if(list.isEmpty())
        list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+""+unit;
}

};

#endif // _QAEHELPERS_H_
