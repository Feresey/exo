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

#include <QtDBus>

#include "dbus.h"
#include "dbus/exoobject.h"
//MPRISv2
#include "dbus/rootobject.h"
#include "dbus/playerobject.h"

DBus::DBus(QObject *parent) : QObject(parent)
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject(QStringLiteral("/Exo"), new ExoObject(this),
                              QDBusConnection::ExportAllContents);
    if(!connection.registerService(QStringLiteral("tk.loimu.exo")))
        qFatal("DBus: service registration failed");
    new RootObject(this);
    new PlayerObject(this);
    connection.registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this);
    if(!connection.registerService(
                QStringLiteral("org.mpris.MediaPlayer2.exo")))
        qFatal("DBus: MPRISv2 service registration failed");
}

DBus::~DBus()
{
    QDBusConnection::sessionBus().unregisterService(
                QStringLiteral("tk.loimu.exo"));
    QDBusConnection::sessionBus().unregisterService(
                QStringLiteral("org.mpris.MediaPlayer2.exo"));
}
