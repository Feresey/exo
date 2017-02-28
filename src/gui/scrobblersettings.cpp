/* ========================================================================
*    Copyright (C) 2013-2017 Blaze <blaze@vivaldi.net>
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

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "lastfm/scrobblerauth.h"
#include "scrobblersettings.h"

ScrobblerSettings::ScrobblerSettings(QWidget *parent) : BaseDialog(parent),
    success(false)
{
    resize(500, 88);
    setWindowTitle(tr("Scrobbler Settings"));
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setPlaceholderText(tr("login"));
    horizontalLayout->addWidget(usernameLineEdit);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setPlaceholderText(tr("password"));
    horizontalLayout->addWidget(passwordLineEdit);
    verticalLayout->addLayout(horizontalLayout);
    QHBoxLayout* horizontalLayout2 = new QHBoxLayout();
    label = new QLabel(this);
    horizontalLayout2->addWidget(label);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(
                QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    horizontalLayout2->addWidget(buttonBox);
    verticalLayout->addLayout(horizontalLayout2);
    scrobblerAuth = new ScrobblerAuth(this);
    connect(scrobblerAuth, SIGNAL(failed(QString)), SLOT(authFail(QString)));
    connect(scrobblerAuth, SIGNAL(configured()), SLOT(authSuccess()));
    connect(usernameLineEdit, SIGNAL(returnPressed()), SLOT(auth()));
    connect(usernameLineEdit, SIGNAL(textChanged(QString)),
            label, SLOT(clear()));
    connect(passwordLineEdit, SIGNAL(returnPressed()), SLOT(auth()));
    connect(passwordLineEdit, SIGNAL(textChanged(QString)),
            label, SLOT(clear()));
    connect(buttonBox, SIGNAL(accepted()), SLOT(auth()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(close()));
}

ScrobblerSettings::~ScrobblerSettings() {
    emit configured(success);
}

void ScrobblerSettings::auth() {
    scrobblerAuth->auth(usernameLineEdit->text(), passwordLineEdit->text());
}

void ScrobblerSettings::authFail(const QString& errmsg) {
    label->setText(errmsg);
}

void ScrobblerSettings::authSuccess() {
    success = true;
    this->close();
}
