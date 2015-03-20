// Copyright 2007 "Gilles Degottex"

// This file is part of "fmit"

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

#include <iostream>
using namespace std;
#include <qsettings.h>
#include "AutoQSettings.h"

AutoQSettings::AutoQSettings(const QString& domain, const QString& product, const QString& setting_version)
	: QSettings(QSettings::UserScope, domain, product)
{
	beginGroup(QString("/")+product+setting_version+"/");
    cerr << "AutoQSettings: " << fileName().toStdString() << endl;
}

void AutoQSettings::add(QCheckBox* el)
{
	assert(el->objectName()!="");
	m_elements_checkbox.push_back(el);
}
void AutoQSettings::add(QSpinBox* el)
{
	assert(el->objectName()!="");
	m_elements_spinbox.push_back(el);
}
void AutoQSettings::add(QLineEdit* el)
{
	assert(el->objectName()!="");
	m_elements_lineedit.push_back(el);
}
void AutoQSettings::add(QComboBox* el)
{
	assert(el->objectName()!="");
	m_elements_combobox.push_back(el);
}
void AutoQSettings::add(QGroupBox* el)
{
	assert(el->objectName()!="");
	m_elements_qgroupbox.push_back(el);
}
void AutoQSettings::add(QRadioButton* el)
{
	assert(el->objectName()!="");
	m_elements_qradiobutton.push_back(el);
}

void AutoQSettings::save(QCheckBox* el)
{
	setValue(el->objectName(), el->isChecked());
}
void AutoQSettings::load(QCheckBox* el)
{
	el->setChecked(value(el->objectName(), el->isChecked()).toBool());
}

void AutoQSettings::save()
{
	beginGroup("Auto/");

	for(list<QCheckBox*>::iterator it=m_elements_checkbox.begin(); it!=m_elements_checkbox.end(); it++)
		setValue((*it)->objectName(), (*it)->isChecked());

	for(list<QSpinBox*>::iterator it=m_elements_spinbox.begin(); it!=m_elements_spinbox.end(); it++)
		setValue((*it)->objectName(), (*it)->value());

	for(list<QLineEdit*>::iterator it=m_elements_lineedit.begin(); it!=m_elements_lineedit.end(); it++)
		setValue((*it)->objectName(), (*it)->text());

	for(list<QComboBox*>::iterator it=m_elements_combobox.begin(); it!=m_elements_combobox.end(); it++)
		setValue((*it)->objectName(), (*it)->currentIndex());

	for(list<QGroupBox*>::iterator it=m_elements_qgroupbox.begin(); it!=m_elements_qgroupbox.end(); it++)
		setValue((*it)->objectName(), (*it)->isChecked());

	for(list<QRadioButton*>::iterator it=m_elements_qradiobutton.begin(); it!=m_elements_qradiobutton.end(); it++)
		setValue((*it)->objectName(), (*it)->isChecked());

	endGroup();
}
void AutoQSettings::load()
{
	beginGroup("Auto/");

	for(list<QCheckBox*>::iterator it=m_elements_checkbox.begin(); it!=m_elements_checkbox.end(); it++)
		(*it)->setChecked(value((*it)->objectName(), (*it)->isChecked()).toBool());

	for(list<QSpinBox*>::iterator it=m_elements_spinbox.begin(); it!=m_elements_spinbox.end(); it++)
		(*it)->setValue(value((*it)->objectName(), (*it)->value()).toInt());

	for(list<QLineEdit*>::iterator it=m_elements_lineedit.begin(); it!=m_elements_lineedit.end(); it++)
		(*it)->setText(value((*it)->objectName(), ((*it)->text())).toString());

	for(list<QComboBox*>::iterator it=m_elements_combobox.begin(); it!=m_elements_combobox.end(); it++)
		(*it)->setCurrentIndex(value((*it)->objectName(), (*it)->currentIndex()).toInt());

	for(list<QGroupBox*>::iterator it=m_elements_qgroupbox.begin(); it!=m_elements_qgroupbox.end(); it++)
		(*it)->setChecked(value((*it)->objectName(), (*it)->isChecked()).toBool());

	for(list<QRadioButton*>::iterator it=m_elements_qradiobutton.begin(); it!=m_elements_qradiobutton.end(); it++)
		(*it)->setChecked(value((*it)->objectName(), (*it)->isChecked()).toBool());

	endGroup();
}
void AutoQSettings::clear()
{
	beginGroup("Auto/");

	for(list<QCheckBox*>::iterator it=m_elements_checkbox.begin(); it!=m_elements_checkbox.end(); it++)
		remove((*it)->objectName());

	for(list<QSpinBox*>::iterator it=m_elements_spinbox.begin(); it!=m_elements_spinbox.end(); it++)
		remove((*it)->objectName());

	for(list<QLineEdit*>::iterator it=m_elements_lineedit.begin(); it!=m_elements_lineedit.end(); it++)
		remove((*it)->objectName());

	for(list<QComboBox*>::iterator it=m_elements_combobox.begin(); it!=m_elements_combobox.end(); it++)
		remove((*it)->objectName());

    for(list<QGroupBox*>::iterator it=m_elements_qgroupbox.begin(); it!=m_elements_qgroupbox.end(); it++)
        remove((*it)->objectName());

    for(list<QRadioButton*>::iterator it=m_elements_qradiobutton.begin(); it!=m_elements_qradiobutton.end(); it++)
        remove((*it)->objectName());

    endGroup();
}
