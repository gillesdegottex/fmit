// Copyright 2005 "Gilles Degottex"

// This file is part of "Music"

// "Music" is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "Music" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef _View_h_
#define _View_h_

#include <list>
using namespace std;
#include <qframe.h>
#include <qmenu.h>
#include <qaction.h>
#include <qlabel.h>
#include "../AutoQSettings.h"

class View
{
	static list<View*> s_views;

protected:
	struct Title : QLabel
	{
		Title(const QString& text, QWidget* parent)	: QLabel(text, parent)
			{setAlignment(Qt::AlignCenter);}
	};

	QString m_name;
	QWidget* m_parent;
	QMenu m_popup_menu;

	View(const QString& name, QWidget* parent);
	virtual void mouseReleaseEvent(QMouseEvent* e);

	virtual ~View() {}

  public:
	static AutoQSettings* s_settings;
	const QString& getName()	{return m_name;}

    int getNbView() const		{return int(s_views.size());}

	// settings
	QAction* setting_show;
	virtual void save(){}
	virtual void load(){}
	virtual void clearSettings(){}

	static void saveAll();
	static void loadAll();
	static void clearAllSettings();
};

//class LabeledSpinBox : QWidget
//{
//  public:
//	LabeledSpinBox(int min, int max, int step, QWidget* parent);
//};

#endif // _View_h_

