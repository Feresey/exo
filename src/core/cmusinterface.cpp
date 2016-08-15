/* ========================================================================
*    Copyright (C) 2013-2016 Blaze <blaze@vivaldi.net>
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

#include <QStringList>
#include <QTime>

#include "cmusinterface.h"

CmusInterface::CmusInterface(QObject *parent) : PlayerInterface(parent),
    cli(QLatin1String("cmus-remote"))
{
    if(!isPlayerRunning())
        runPlayer();
    startTimer(1000);
}

QString CmusInterface::id() {
    return QLatin1String("Cmus");
}

bool CmusInterface::isPlayerRunning() {
    return getOutput(QLatin1String("pidof"), QStringList()
                     << QLatin1String("cmus")).length() > 1;
}

bool CmusInterface::runPlayer() {
    QString term = QLatin1String("x-terminal-emulator");
    // falling back to xterm if there're no alternatives
    if(getOutput(QLatin1String("which"), QStringList() << term).length() < 1)
        term = QLatin1String("xterm");
    return execute(term, QStringList() << QLatin1String("-e")
                   << QLatin1String("cmus"));
}

#define SEND_COMMAND(__method, __option)\
    bool CmusInterface::__method() {\
        return execute(cli, QStringList() << __option);\
    }

SEND_COMMAND(play, QLatin1String("-p"))
SEND_COMMAND(pause,QLatin1String("-u"))
SEND_COMMAND(playPause, QLatin1String("-u"))
SEND_COMMAND(prev, QLatin1String("-r"))
SEND_COMMAND(next, QLatin1String("-n"))
SEND_COMMAND(stop, QLatin1String("-s"))

bool CmusInterface::quit() {
    return execute(cli, QStringList() << QLatin1String("-C")
                   << QLatin1String("quit"));
}

bool CmusInterface::jump(int pos) {
    return execute(cli, QStringList() << QLatin1String("-k")
                   << QString::number(pos));
}

bool CmusInterface::seek(int offset) {
    QString o = (offset > 0) ? QLatin1String("+") +
                               QString::number(offset) : QString::number(offset);
    return execute(cli, QStringList() << QLatin1String("-k") << o);
}

bool CmusInterface::volume(int lev) {
    return execute(cli, QStringList() << QLatin1String("-v")
                   << QString::number(lev) + QLatin1String("%"));
}

bool CmusInterface::changeVolume(int delta) {
    QString d = ((delta > 0) ? QLatin1String("+")
                               + QString::number(delta) : QString::number(delta))
            + QLatin1String("%");
    return execute(cli, QStringList() << QLatin1String("-v") << d);
}

bool CmusInterface::showPlayer() {
    return runPlayer();
}

bool CmusInterface::openUri(const QString file) {
    return execute(cli, QStringList() << QLatin1String("-q") << file);
}

bool CmusInterface::appendFile(QStringList files) {
    return execute(cli, QStringList() << QLatin1String("-q") << files);
}

QString CmusInterface::find(QString string, const QString regEx) {
    QRegExp findRgx(regEx);
    findRgx.setMinimal(true);
    findRgx.indexIn(string);
    return findRgx.cap(1);
}

State CmusInterface::getInfo() {
    QString info = getOutput(cli, QStringList() << QLatin1String("-Q"));
    if(info.size() < 1)
        return Offline;
    QString string = find(info, QLatin1String("status\\s(.*)\\n"));
    if(string == QLatin1String("stopped"))
        return Stop;
    State state = Offline;
    if(string == QLatin1String("playing"))
        state = Play;
    if(string == QLatin1String("paused"))
        state = Pause;
    track.artist = find(info, QLatin1String("tag\\sartist\\s(.*)\\n"));
    track.song = find(info, QLatin1String("tag\\stitle\\s(.*)\\n"));
    track.album = find(info, QLatin1String("tag\\salbum\\s(.*)\\n"));
    track.file = find(info, QLatin1String("file\\s(.*)\\n"));
    track.totalSec = find(info, QLatin1String("duration\\s(.*)\\n")).toInt();
    track.currSec = find(info, QLatin1String("position\\s(.*)\\n")).toInt();
    track.totalTime = QTime().addSecs(track.totalSec).toString(
                QLatin1String("mm:ss"));
    track.currTime = QTime().addSecs(track.currSec).toString(
                QLatin1String("mm:ss"));
    track.number = find(info,
                        QLatin1String("tag\\stracknumber\\s(.*)\\n")).toInt();
    track.title = track.artist.isEmpty() ? track.song : track.artist
                                           + QLatin1String(" - ") + track.song;
    if(!track.file.startsWith(QLatin1String("http")))
        return state;
    QString song = find(info, QLatin1String("stream\\s(.*)\\n"));
    track.title += QLatin1String("<br />") + song;
    track.totalSec = 8*60;
    if(!song.isEmpty()) {
        QRegExp artistRgx(QLatin1String("^(.*)\\s-\\s"));
        artistRgx.setMinimal(true);
        artistRgx.indexIn(song);
        track.artist = artistRgx.cap(1);
        QRegExp titleRgx(QLatin1String("\\s-\\s(.*)$"));
        titleRgx.indexIn(song);
        track.song = titleRgx.cap(1);
    }
    return state;
}
