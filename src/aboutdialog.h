/* ========================================================================
*    Copyright (C) 2013-2015 Blaze <blaze@open.by>
*
*    This file is part of eXo.
*
*    eXo is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    eXo is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with eXo.  If not, see <http://www.gnu.org/licenses/>.
* ======================================================================== */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QWidget>

#include "ui_aboutdialog.h"

class AboutDialog : public QWidget
{
    Q_OBJECT

    Ui::AboutDialog ui;
    void addText();
    void keyPressEvent(QKeyEvent *e);

public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();
signals:
    void destroyed(bool);
};

#endif // ABOUTDIALOG_H
