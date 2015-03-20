#include "DialView.h"

#include <iostream>
using namespace std;
#include <qpainter.h>
#include <qmenu.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qwidgetaction.h>
#include <qboxlayout.h>
#include <Music/Music.h>

DialView::DialView(QWidget* parent)
: QFrame(parent)
, View(tr("Dial"), this)
{
	m_error = 0.0f;
	m_min_error = 0.0f;
	m_max_error = 0.0f;

	// settings
	setting_showTolerance = new QAction(tr("Show tolerance"), this);
	setting_showTolerance->setCheckable(true);
	setting_showTolerance->setChecked(true);
	connect(setting_showTolerance, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_showTolerance);

	setting_showTrace = new QAction(tr("Show trace"), this);
	setting_showTrace->setCheckable(true);
	setting_showTrace->setChecked(true);
	connect(setting_showTrace, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_showTrace);

	setting_useCents = new QAction(tr("Use cents"), this);
	setting_useCents->setCheckable(true);
	setting_useCents->setChecked(true);
	connect(setting_useCents, SIGNAL(toggled(bool)), this, SLOT(update()));
	m_popup_menu.addAction(setting_useCents);

	QHBoxLayout* scaleActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* scaleActionTitle = new QLabel(tr("Scale range"), &m_popup_menu);
	scaleActionLayout->addWidget(scaleActionTitle);

	setting_spinScale = new QSpinBox(&m_popup_menu);
	setting_spinScale->setMinimum(5);
	setting_spinScale->setMaximum(100);
	setting_spinScale->setSingleStep(1);
	setting_spinScale->setValue(50);
	setting_spinScale->setToolTip(tr("Scale range (in cents)"));
	connect(setting_spinScale, SIGNAL(valueChanged(int)), this, SLOT(update()));
	scaleActionLayout->addWidget(setting_spinScale);

	QWidget* scaleActionWidget = new QWidget(&m_popup_menu);
	scaleActionWidget->setLayout(scaleActionLayout);

	QWidgetAction* scaleAction = new QWidgetAction(&m_popup_menu);
	scaleAction->setDefaultWidget(scaleActionWidget);
	m_popup_menu.addAction(scaleAction);
}

void DialView::save()
{
	s_settings->setValue("showTolerance", setting_showTolerance->isChecked());
	s_settings->setValue("showTrace", setting_showTrace->isChecked());
	s_settings->setValue("useCents", setting_useCents->isChecked());
	s_settings->setValue("spinScale", setting_spinScale->value());
}
void DialView::load()
{
	setting_showTolerance->setChecked(s_settings->value("showTolerance", setting_showTolerance->isChecked()).toBool());
	setting_showTrace->setChecked(s_settings->value("showTrace", setting_showTrace->isChecked()).toBool());
	setting_useCents->setChecked(s_settings->value("useCents", setting_useCents->isChecked()).toBool());
	setting_spinScale->setValue(s_settings->value("spinScale", setting_spinScale->value()).toInt());
}
void DialView::clearSettings()
{
	s_settings->remove("showTolerance");
	s_settings->remove("showTrace");
	s_settings->remove("useCents");
	s_settings->remove("spinScale");
}

void DialView::drawTextTickCent(QPainter& p, int bigw, int bigh, int r)
{
	QString txt;
// 	double deg = 0.0;

	int bw = bigw/2-p.fontMetrics().height()+p.fontMetrics().height()/2+2;
	int bh = bigh-p.fontMetrics().height()+p.fontMetrics().height()/2+2;

	// only work within range that is a pure multiple of r
	int range = int(setting_spinScale->value()/r)*r;

	float scale = 50.0f/setting_spinScale->value()/100.0f;
	for(int i=-range; i<=range; i+=r)
	{
		txt = QString::number(i);
		float angle = (150.0f/180.0f)*i*scale*Math::Pi + Math::Pi/2;
		QPoint pos(int(-bw*cos(angle)),int(-bh*sin(angle)));

		p.translate(pos.x(), pos.y());

		if(height()>width())	p.rotate(90);
		p.drawText(-p.fontMetrics().width(txt)/2,p.fontMetrics().height()/2, txt);
		if(height()>width())	p.rotate(-90);

		p.translate(-pos.x(), -pos.y());
	}
}
void DialView::drawTicksCent(QPainter& p, int bigw, int bigh, int r, int ticks_size, int h)
{
	int sw = int(bigw/2.0f-p.fontMetrics().height()-ticks_size);
	int sh = bigh-p.fontMetrics().height()-ticks_size;
	int bw = sw + h;
	int bh = sh + h;

	// only work within range that is a pure multiple of r
	float range = int(setting_spinScale->value()/r)*r;

	float scale = 50.0f/setting_spinScale->value()/100.0f;
	for(float i=-range; i<=range; i+=r)
 	{
		float angle = (150.0f/180.0f)*i*scale*Math::Pi + Math::Pi/2;
		p.drawLine(-QPoint(int(sw*cos(angle)),int(sh*sin(angle))), -QPoint(int(bw*cos(angle)),int(bh*sin(angle))));
	}
}
void DialView::drawTextTickFrac(QPainter& p, int bigw, int bigh, int num, int den)
{
	QString txt;
	double deg = 0.0;
	if(num==0 || den==0)
		txt = "0";
	else
	{
		deg = num*150/den;
		txt = QString::number(den*(50/setting_spinScale->value()));
	}

	int bw = bigw/2-p.fontMetrics().height()+p.fontMetrics().height()/2+2;
	int bh = bigh-p.fontMetrics().height()+p.fontMetrics().height()/2+2;
	float angle = deg/180.0f*Math::Pi + Math::Pi/2;
	QPoint pos(int(-bw*cos(angle)),int(-bh*sin(angle)));

	p.translate(pos.x(), pos.y());

	if(height()>width())	p.rotate(90);
	p.drawText(-p.fontMetrics().width(txt)/2,p.fontMetrics().height()/2, txt);
	if(height()>width())	p.rotate(-90);

	p.translate(-pos.x(), -pos.y());
}
void DialView::drawTicksFrac(QPainter& p, int bigw, int bigh, float r, int ticks_size, int h)
{
	int sw = bigw/2-p.fontMetrics().height()-ticks_size;
	int sh = bigh-p.fontMetrics().height()-ticks_size;
	int bw = sw + h;
	int bh = sh + h;
	for(float i=-0.5f+r; i<0.5f; i+=2*r)
	{
		float angle = (150.0f/180.0f)*i*Math::Pi + Math::Pi/2;
		p.drawLine(-QPoint(int(sw*cos(angle)),int(sh*sin(angle))), -QPoint(int(bw*cos(angle)),int(bh*sin(angle))));
	}
}

void DialView::setError(float error)
{
	m_error = error;
	repaint();
}

void DialView::paintEvent(QPaintEvent* event)
{
	QPainter p(this);

	float human_tol = Music::f2hf(441.0,440.0);			// TODO
	float scale;
	if(setting_useCents->isChecked())
		scale = 50.0f/setting_spinScale->value();
	else
		scale = int(50/setting_spinScale->value());

	int bigw = width();
	int bigh = height();

 	QPoint c(width()/2,height());
	QPoint unity_center = c+QPoint(p.fontMetrics().width("cent"),0);
	QString unity;
	if(setting_useCents->isChecked())	unity = "cents";
	else							unity = "1/x";
	if(height()>width())
	{
		c = QPoint(width(), height()/2);
		bigw = height();
		bigh = width();
		unity_center = c+QPoint(-p.fontMetrics().width(unity),-p.fontMetrics().height());
	}
	int ticks_size = bigh/10;

	// draw unity
	p.setPen(QColor(0,0,0));
	p.drawText(unity_center, unity);

	p.translate(c.x(), c.y());

	if(height()>width())
		p.rotate(-90);

	// draw pie
	int pie_width = bigw/2-p.fontMetrics().height()-ticks_size-2;
	int pie_height = bigh-p.fontMetrics().height()-ticks_size-2;
	p.setBrush(QColor(123,205,145));
	QPen pen1 = p.pen();
	pen1.setColor(QColor(83,165,105));
//	pen1.setStyle(Qt::DashLine);
//	pen1.setWidth(2);
	p.setPen(pen1);
	if(setting_showTolerance->isChecked())
	{
	    float pie_deg = (150.0f/180.0f)*scale*human_tol*180;
	    p.drawPie(-pie_width,-pie_height,2*pie_width,2*pie_height, int((90-pie_deg)*16),int(2*pie_deg*16));
	}

	// draw min max
	if((m_error>-1.0f && m_error<1.0f) && setting_showTrace->isChecked() && m_max_error-m_min_error>0.0)
	{
		int line_width = ticks_size/4;
		int mm_height = pie_height - line_width/2;
		int mm_width = pie_width - line_width/2;
		QPen pen3 = p.pen();
		pen3.setColor(QColor(128,128,128));
		pen3.setWidth(line_width);
		p.setPen(pen3);

		float mm_degf = (150.0f/180.0f)*scale*180;
		p.drawArc(-mm_width,-mm_height,2*mm_width,2*mm_height,int((90-mm_degf*m_min_error)*16),int((-mm_degf*(m_max_error-m_min_error))*16));
	}

	// draw text marks
	p.setPen(QColor(0,0,0));
	if(setting_useCents->isChecked())
	{
		int grad = 10;
		if(setting_spinScale->value() <= 25) grad=5;
		if(setting_spinScale->value() <= 10) grad=1;
		drawTextTickCent(p, bigw, bigh, grad);
	}
	else
	{
		drawTextTickFrac(p, bigw, bigh, 0,1);
		drawTextTickFrac(p, bigw, bigh, 1,2);
		drawTextTickFrac(p, bigw, bigh, -1,2);
		drawTextTickFrac(p, bigw, bigh, 1,4);
		drawTextTickFrac(p, bigw, bigh, -1,4);
		drawTextTickFrac(p, bigw, bigh, 1,8);
		drawTextTickFrac(p, bigw, bigh, -1,8);
		drawTextTickFrac(p, bigw, bigh, 1,16);
		drawTextTickFrac(p, bigw, bigh, -1,16);
	}

	// draw ticks
	int sw = bigw/2-p.fontMetrics().height()-ticks_size;
	int sh = bigh-p.fontMetrics().height()-ticks_size;
	int bw = sw + ticks_size;
	int bh = sh + ticks_size;
	if(setting_useCents->isChecked())
	{
		drawTicksCent(p, bigw, bigh, 10, ticks_size, ticks_size);
		drawTicksCent(p, bigw, bigh, 5, ticks_size, int(ticks_size * 0.6));
		drawTicksCent(p, bigw, bigh, 1, ticks_size, int(ticks_size * 0.3));
	}
	else
	{
		for(float i=-1.0f; i<=1.0f; i+=0.25f)
		{
			float angle = (150.0f/360.0f)*i*Math::Pi + Math::Pi/2;
			p.drawLine(-QPoint(int(sw*cos(angle)),int(sh*sin(angle))), -QPoint(int(bw*cos(angle)),int(bh*sin(angle))));
		}
		drawTicksFrac(p, bigw, bigh, 0.125f, ticks_size, 2*ticks_size/3);
		drawTicksFrac(p, bigw, bigh, 0.0625f, ticks_size, ticks_size/2);
		drawTicksFrac(p, bigw, bigh, 0.03125f, ticks_size, ticks_size/3);
	}

	// draw arrow
	if(m_error>-1.0f && m_error<1.0f)
	{
		float angle = (150.0f/180.0f)*scale*m_error*Math::Pi + Math::Pi/2;
		QPoint final_point = -QPoint(int((pie_width-2)*cos(angle)),int((pie_height-2)*sin(angle)));
		QPoint mid_point = 9*final_point/10;
		QPen pen2 = p.pen();
		pen2.setWidth(3);
		pen2.setColor(QColor(64,64,64));
		p.setPen(pen2);
		p.drawLine(QPoint(0,0), mid_point);
		pen2.setWidth(1);
		p.setPen(pen2);
		p.drawLine(mid_point, final_point);
	}
}
