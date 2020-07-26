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


#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include <QDialog>

namespace Ui {
class AboutBox;
}

class AboutBox : public QDialog
{
    Q_OBJECT

public:
    explicit AboutBox(QWidget *parent = 0);
    ~AboutBox();

private:
    Ui::AboutBox *ui;
};

#endif // ABOUTBOX_H
