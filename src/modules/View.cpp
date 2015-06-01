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


#include "View.h"

#include <cassert>
#include <iostream>
using namespace std;
#include <qtooltip.h>
#include <qevent.h>
#include <qwidgetaction.h>

list<View*> View::s_views;
AutoQSettings* View::s_settings = NULL;

void View::saveAll()
{
	assert(s_settings!=NULL);

	for(list<View*>::iterator it=s_views.begin(); it!=s_views.end(); ++it)
	{
		s_settings->beginGroup((*it)->getName());
		s_settings->setValue("show", (*it)->setting_show->isChecked());
		(*it)->save();
		s_settings->endGroup();
	}
}
void View::loadAll()
{
	assert(s_settings!=NULL);

	for(list<View*>::iterator it=s_views.begin(); it!=s_views.end(); ++it)
	{
		s_settings->beginGroup((*it)->getName());
		(*it)->setting_show->setChecked(s_settings->value("show", (*it)->setting_show->isChecked()).toBool());
		(*it)->load();
		s_settings->endGroup();
	}
}
void View::clearAllSettings()
{
	assert(s_settings!=NULL);

	for(list<View*>::iterator it=s_views.begin(); it!=s_views.end(); ++it)
	{
		s_settings->beginGroup((*it)->getName());
		s_settings->remove("show");
		(*it)->clearSettings();
		s_settings->endGroup();
	}
}

// ------------------------- instances ---------------------------

View::View(const QString& name, QWidget* parent)
: m_name(name)
, m_parent(parent)
, m_popup_menu(parent)
{
	// settings
	QWidgetAction* caption = new QWidgetAction(&m_popup_menu);
	caption->setDefaultWidget(new Title(QString("<b>%1</b>").arg(m_name), &m_popup_menu));
	m_popup_menu.addAction(caption);

	m_popup_menu.addSeparator();

	setting_show = new QAction(m_name, parent);
	setting_show->setCheckable(true);
	parent->connect(setting_show, SIGNAL(toggled(bool)), parent, SLOT(setShown(bool)));
	setting_show->setToolTip(QObject::tr("Show \"%1\" view").arg(m_name));

	parent->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	s_views.push_back(this);
}

void View::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->button()==Qt::RightButton)
		m_popup_menu.exec(e->globalPos());
}

// ----------------------- LabeledSpinBox ------------------------

//LabeledSpinBox::LabeledSpinBox(int min, int max, int step, QWidget* parent)
//: QWidget(parent)
//{
//
//}

