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

#include <QByteArray>
#include <QNetworkProxyFactory>

#include "core/singleinstance.h"
#include "core/exo.h"

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(exo);
    bool useGui = true;
    for(int i=1; i<argc; i++) {
        QByteArray arg = argv[i];
        if(arg == QByteArray("-d") || arg == QByteArray("-b")
                || arg == QByteArray("--background")) {
            useGui = false;
        }
        if(arg == QByteArray("-f") || arg == QByteArray("--force-reauth")) {
            useGui = false;
            Exo::forceReauth();
        }
    }
    QCoreApplication::setOrganizationName(QLatin1String("exo"));
    QCoreApplication::setApplicationName(QLatin1String("eXo"));
    QCoreApplication::setApplicationVersion(QLatin1String("0.7"));
    SingleInstance inst;
    if(!inst.isUnique()) {
        qWarning("Application is already running");
        return 1;
    }
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    Exo app(argc, argv, useGui);
    return app.exec();
}
