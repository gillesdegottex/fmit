#ifndef QTHELPER_H
#define QTHELPER_H

#include <QRectF>

#include <iostream>

#include <QThread>
#include <QDateTime>
#include <QColor>
#include <QGraphicsView>
#include <QTextStream>
#include <QScrollBar>
#include <QAudioFormat>

#ifdef __MINGW32__
    #define COMPILER "MinGW"
#elif (defined(__GNUC__) || defined(__GNUG__))
    #define COMPILER "GCC"
#elif defined(_MSC_VER)
    #define COMPILER "MSVC"
#endif


// Check if compiling using MSVC (Microsoft compiler)
#ifdef _MSC_VER
    // The following is necessary for MSVC 2012
    template<class Type> inline Type log2(Type v) {return std::log(v)/std::log(2.); }
#endif

#define QUOTE(name) #name
#ifdef _MSC_VER
    #define STR(val) QUOTE(val,"")
#else
    #define STR(val) QUOTE(val)
#endif


#define COUTD std::cout << QThread::currentThreadId() << " " << QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString("hh:mm:ss.zzz             ").toLocal8Bit().constData() << " " << __FILE__ << ":" << __LINE__ << " "

//#define COUTD QTextStream(stdout) << QThread::currentThreadId() << " " << QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString("hh:mm:ss.zzz             ").toLocal8Bit().constData() << " " << __FILE__ << ":" << __LINE__ << " "

#define FLAG COUTD << std::endl;

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
        Q_UNUSED(e)
        setCursor(Qt::ArrowCursor);
    }
};

//            COUTD << snd->m_dftparams.nl << " " << snd->m_dftparams.nr << " " << snd->m_dftparams.winlen << " " << snd->m_dftparams.dftlen << " " << snd->m_dftparams.ampscale << " " << snd->m_dftparams.delay << endl;

#endif // QTHELPER_H
