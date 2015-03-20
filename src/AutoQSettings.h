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

#ifndef _AutoQSettings_h_
#define _AutoQSettings_h_

#include <assert.h>
#include <list>
#include <qsettings.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>

class AutoQSettings : public QSettings
{
	std::list<QCheckBox*> m_elements_checkbox;
	std::list<QSpinBox*> m_elements_spinbox;
	std::list<QLineEdit*> m_elements_lineedit;
	std::list<QComboBox*> m_elements_combobox;
	std::list<QGroupBox*> m_elements_qgroupbox;
	std::list<QRadioButton*> m_elements_qradiobutton;

  public:
	AutoQSettings(const QString& domain, const QString& product, const QString& setting_version);

	void add(QCheckBox* el);
	void add(QSpinBox* el);
	void add(QLineEdit* el);
	void add(QComboBox* el);
	void add(QGroupBox* el);
	void add(QRadioButton* el);

	void save(QCheckBox* el);
	void load(QCheckBox* el);

	void save();
	void load();
	void clear();
};

#endif // _AutoQSettings_h_
