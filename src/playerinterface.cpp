/* ========================================================================
*    Copyright (C) 2013-2014 Blaze <blaze@jabster.pl>
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

#include <QProcess>
#include <QTimer>
#include <QSettings>

#include "playerinterface.h"

const char* PlayerInterface::settingsGroup = "player";

PlayerInterface::PlayerInterface(QObject *parent) : QObject(parent),
    m_artist(QString()),
    m_title(QString())
{
    if(!isServerRunning())
        runServer();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

bool PlayerInterface::isServerRunning() {
    if(execute("pidof", "mocp").length() > 1)
        return true;
    else
        return false;
}

QString PlayerInterface::execute(QString program, QString option) {
    QProcess proc;
    proc.start(program, QStringList() << option);
    proc.waitForFinished(-1);
    return QString::fromUtf8(proc.readAllStandardOutput());
}

void PlayerInterface::sendOption(QString option) {
    QProcess proc;
    proc.startDetached("mocp", QStringList() << option);
}

void PlayerInterface::runServer() {
    sendOption("-S");
}

void PlayerInterface::play() {
    sendOption("-p");
}

void PlayerInterface::pause() {
    sendOption("-G");
}

void PlayerInterface::prev() {
    sendOption("-r");
}

void PlayerInterface::next() {
    sendOption("-f");
}

void PlayerInterface::stop() {
    sendOption("-s");
}

void PlayerInterface::quit() {
    QSettings settings;
    settings.beginGroup(settingsGroup);
    if(settings.value("quit").toBool())
        sendOption("-x");
}

void PlayerInterface::volu() {
    sendOption("-v+2");
}

void PlayerInterface::vold() {
    sendOption("-v-2");
}

void PlayerInterface::rewd() {
    sendOption("-k-10");
}

void PlayerInterface::frwd() {
    sendOption("-k10");
}

void PlayerInterface::update() {
    QStringList list = execute("mocp", "-i").split(QRegExp("[\r\n]"),
                                                   QString::SkipEmptyParts);
    list.replaceInStrings(QRegExp("(\\w+:\\s)+(.*)"), "\\2");
    int listSize = list.size();
    static bool listened = true;
    static QString nowPlaying = QString();
    static QString message = QString();
    static QString totalTime = QString();
    static QString path = QString();
    static int totalSec = 0;
    static const int streamListSize = 11;
    QString currentTime = QString();
    // the following condition is true if file or stream is playing
    if(listSize >= streamListSize) {
        int currentSec = list.at(10).toInt();
        currentTime = list.at(9);
        // condition is true if track have changed
        if(nowPlaying != list.at(2)) {
            nowPlaying = list.at(2);
            message = nowPlaying;
            m_title = list.at(4);
            // condition is true for radio streams
            if(listSize == streamListSize) {
                totalSec = 8*60;
                if(!m_title.isEmpty()) {
                    m_artist = m_title;
                    m_artist.replace(QRegExp("^(.+)\\s-\\s.*"), "\\1");
                    m_title.replace(QRegExp("^.+\\s-\\s(.*)"), "\\1");
                }
            }
            else {
                m_artist = list.at(3);
                totalSec = list.at(8).toInt();
                totalTime = list.at(6);
                path = list.at(1);
            }
            // 1st signal for scrobbler
            emit trackChanged(m_artist, m_title, totalSec);
        }
        else if(listSize > streamListSize) {
            if(listened && ((currentSec < totalSec/2 && totalSec < 8*60)||
                                   (currentSec < 4*60 && totalSec > 8*60))) {
                listened = false;
            }
            else if(!listened && (currentSec > totalSec/2 ||
                                    (currentSec > 4*60 && totalSec > 8*60))) {
                listened = true;
                QString album = list.at(5);
                // 2nd signal for scrobbler
                emit trackListened(m_artist, m_title, album, totalSec);
            }
        }
    }
    else {
        if(listSize == 0)
            message = tr("Player is not running, make a doubleclick.");
        else if (listSize == 1)
            message = tr("Stopped");
        currentTime = QString();
        path = QString();
    }
    // signal for trayicon
    emit updateStatus(message, currentTime, totalTime, path);
}

void PlayerInterface::openWindow() {
    QProcess proc;
    proc.startDetached("x-terminal-emulator", QStringList() << "-e" << "mocp");
}

QString PlayerInterface::artist() {
    return QString(m_artist);
}

QString PlayerInterface::title() {
    return QString(m_title);
}