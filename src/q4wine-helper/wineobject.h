/***************************************************************************
 *   Copyright (C) 2008, 2009, 2010 by Malakhov Alexey                                 *
 *   brezerk@gmail.com                                                     *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef WINEOBJECT_H
#define WINEOBJECT_H

#include <memory>
#include "config.h"

#include <QObject>
#include <QLocalSocket>
#include <QTime>

#include "prefix.h"
#include "logging.h"

#include "q4wine-lib.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX 8192

class WineObject : public QObject
{
Q_OBJECT
public:
    explicit WineObject(QObject *parent = 0);

    void setPrefix(QString prefix);

    void setProgramBinary(QString binary);
    void setProgramArgs(QString args);
    void setProgramDisplay(QString dislpay);
    void setProgramDebug(QString debug);
    void setProgramNice(int nice);
    void setProgramDesktop(QString desktop);
    void setProgramOverride(QString override);
    void setProgramWrkdir(QString wrkdir);

    void setUseConsole(int console);
    void setOverrideDll(QString dll_list);

    void runSys();

private:

    //! This is need for libq4wine-core.so import;
    typedef void *CoreLibPrototype (bool);
        CoreLibPrototype *CoreLibClassPointer;
        std::auto_ptr<corelib> CoreLib;
    QLibrary libq4wine;

    QString createEnvString(void);
    void sendMessage(QString message);

    Prefix db_prefix;
    Logging db_logging;

    QString prefixName;
    QString prefixPath;
    QString prefixDllPath;
    int prefixId;
    QString prefixLoader;
    QString prefixServer;
    QString prefixBinary;

    QString programBinary;
    QString programBinaryName;
    QString programArgs;
    QString programDisplay;
    QString programDebug;
    QString programWrkDir;
    int programNice;
    QString programDesktop;
    QString overrideDllList;

    bool useConsole;

    char *user;

signals:

public slots:

};

#endif // WINEOBJECT_H
