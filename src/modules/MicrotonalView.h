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


#ifndef _MicrotonalView_h_
#define _MicrotonalView_h_

#include <deque>
#include <vector>
#include <list>
using namespace std;
#include <qwidget.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qaction.h>
#ifdef QT3_SUPPORT
#include <q3filedialog.h>
#else
#include <qfiledialog.h>
#endif
#include <qtooltip.h>
#include "View.h"

class MicrotonalView;

class MScale
{
	void load_scala(const QString& file_name);

	QString m_name;
	QString m_path;

  public:
	struct MValue
	{
		bool is_ratio;
		int num;
		int den;
		float ratio;
		float cents;
		QRect bounding_rect;
		MValue(int n, int d) : is_ratio(true), num(n), den(d), ratio(float(num)/den) {}
		MValue(float c) : is_ratio(false), cents(c) {}
	};

	QString getName() {return m_name;}
	QString getPath() {return m_path;}

	enum FileType {AUTO,SCALA};
	MScale(const QString& name);
	MScale(const QString& file_name, FileType type);

	list<MValue> values;

	bool operator != (const MScale& scale);
};

class QRoot : public QPushButton
{
	Q_OBJECT

  private slots:
	void clicked2();

  signals:
	void rootClicked(int);

  public:
	QRoot(MicrotonalView* view, int ht);

	int m_ht;

	virtual ~QRoot(){}
};

class MicrotonalView : public QFrame, public View
{
	Q_OBJECT

	QMenu  ui_scale_menu;

	float m_AFreq;
	float m_tuningFreq;

	void load_default_scales();

	vector<QRoot*> m_roots;
	struct QScaleLabel : QLabel
	{
		MicrotonalView* ui_view;
		float m_htf;
		QScaleLabel(MicrotonalView* view);
		virtual void paintEvent(QPaintEvent* event);
		virtual ~QScaleLabel(){}
	}* ui_scale;

	struct QRatiosLabel : QLabel
	{
		MicrotonalView* ui_view;
		void drawTicks(QPainter* p, float r, int h);
		QRatiosLabel(MicrotonalView* view);
		virtual void paintEvent(QPaintEvent* event);
		virtual ~QRatiosLabel(){}
	}* ui_ratios;

	virtual void mouseReleaseEvent(QMouseEvent* e);

#ifdef QT3_SUPPORT
		struct ScalePreview : QLabel, Q3FilePreview
	{
		ScalePreview(QWidget* parent);
		virtual void previewUrl(const Q3Url& url);
	};
#endif

	int m_old_octave;
	int getOctaveShift();

  protected slots:
	void refreshScaleList();
	void loadScale();
	void scaleActionToggled(bool checked);
	void selectScale(int index);
// 	void octaveChanged(int value);
	void emitTuningFreqChanged();

  public:
	MicrotonalView(QWidget* parent);

	float getTuningFreq()					{return m_tuningFreq;}
	bool hasTuningFreqSelected()			{return m_selected_jivalue!=NULL;}
	QString getTuningNoteName();
	MScale::MValue* m_selected_jivalue;

	int getIndex(MScale* scale);

	~MicrotonalView(){}

	// settings
	QSpinBox* setting_octave;
	QAction* setting_keepRootToLeft;
	int setting_selectedRoot;
	QAction* setting_loadScale;
	vector<MScale*> setting_scales;
	MScale* setting_selectedScale;
	QString setting_lastScalesDirectory;
	virtual void save();
	virtual void load();
	virtual void clearSettings();

  public slots:
	void setAFreq(float AFreq);
	void selectScale(const QString& name);
	void selectRoot(int ht);
	void updateCursor(float freq);
	void notesNameChanged();
	void keepRootToLeft(bool keep);

  signals:
	void tuningFreqChanged(float freq);
};

#endif // _MicrotonalView_h_
