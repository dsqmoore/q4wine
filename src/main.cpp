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

#include "memory"

#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QTextStream>

#ifdef DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QLibrary>

#include "mainwindow.h"

#include <stdlib.h>

#include "stdio.h"
#include <locale.h>

#include "db.h"

#include "qtsingleapplication.h"

//q4wine lib
#include "q4wine-lib.h"

int main(int argc, char *argv[])
{
	QtSingleApplication app(argc, argv);
    QTextStream QErr(stderr);

    QString exec_binary;
    if (app.arguments().count()>2){
        if ((app.arguments().at(1)=="--binary") or (app.arguments().at(1)=="-b")){
            exec_binary = app.arguments().at(2);
            if (app.sendMessage(exec_binary))
                return 0;
        }
    }

    if (app.sendMessage(""))
		return 0;

	//! This is need for libq4wine-core.so import;
	typedef void *CoreLibPrototype (bool);
    CoreLibPrototype *CoreLibClassPointer;
    std::auto_ptr<corelib> CoreLib;
	QLibrary libq4wine;

	// Loading libq4wine-core.so
	libq4wine.setFileName("libq4wine-core");

	if (!libq4wine.load()){
		libq4wine.load();
	}

	// Getting corelib calss pointer
	CoreLibClassPointer = (CoreLibPrototype *) libq4wine.resolve("createCoreLib");
	CoreLib.reset((corelib *)CoreLibClassPointer(true));

	if (!CoreLib.get()){
        QErr<<"[EE] Can't load shared library."<<endl;
		return -1;
	}

    QTranslator qtt;
    qtt.load(CoreLib->getTranslationLang(), QString("%1/share/%2/i18n").arg(APP_PREF).arg(APP_SHORT_NAME));
    app.installTranslator(&qtt);

    if (!CoreLib->isConfigured()){
        Wizard firstSetupWizard(1);
        if (firstSetupWizard.exec()==QDialog::Rejected){
            QErr<<"[EE] App not configured! Re run wizard, or delete q4wine broken config files."<<endl;
            return -1;
        }
    }

    if (!CoreLib->checkDirs()){
        return -1;
    }

	DataBase db;

    if (!db.checkDb()){
        QErr<<"[EE] Can't init database engine."<<endl;
        return -1;
    }

	QTextStream Qcout(stdout);
	int result, startState=0;

	if (app.arguments().count()>1){
        if ((app.arguments().at(1)=="--version") or (app.arguments().at(1)=="-v")){
			Qcout<<QString("%1 %2").arg(APP_SHORT_NAME).arg(APP_VERS)<<endl;
			Qcout<<QString("(Copyright (C) 2008-2009, brezblock core team.")<<endl;
			Qcout<<QString("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.")<<endl;
			Qcout<<QObject::tr("This is free software: you are free to change and redistribute it.")<<endl;
			Qcout<<QObject::tr("There is NO WARRANTY, to the extent permitted by law.")<<endl;
            CoreLib->getBuildFlags();
			Qcout<<QObject::tr("Author: %1.").arg("Malakhov Alexey aka John Brezerk")<<endl;
			return 0;
        } else if ((app.arguments().at(1)=="--minimize") or (app.arguments().at(1)=="-m")) {
			startState = 1;
        } else if ((app.arguments().at(1)=="--binary") or (app.arguments().at(1)=="-b")) {
            //startState = 1;
        } else {
			Qcout<<QObject::tr("Usage:")<<endl;
            Qcout<<QString("  %1 -b <unix_path_to_windown_binary>").arg(APP_SHORT_NAME)<<endl;
            Qcout<<QObject::tr("  %1 [KEY]...").arg(APP_SHORT_NAME)<<endl;
			Qcout<<QObject::tr("GUI utility for wine applications and prefixes management.")<<endl<<endl;
			Qcout<<QObject::tr("KEYs list:")<<endl;
			Qcout<<qSetFieldWidth(25)<<left<<"  -h,  --help"<<QObject::tr("display this help and exit")<<qSetFieldWidth(0)<<endl;
			Qcout<<qSetFieldWidth(25)<<left<<"  -v,  --version"<<QObject::tr("output version information and exit")<<qSetFieldWidth(0)<<endl;
            Qcout<<qSetFieldWidth(25)<<left<<"  -b,  --binary"<<QObject::tr("Open q4wine run dialog for windows binary")<<qSetFieldWidth(0)<<endl;
			Qcout<<qSetFieldWidth(25)<<left<<"  -m,  --minimize"<<QObject::tr("minimize %1 main window on startup").arg(APP_SHORT_NAME)<<qSetFieldWidth(0)<<endl;
			Qcout<<endl;
			Qcout<<QObject::tr("Report %1 bugs to %2").arg(APP_SHORT_NAME).arg(APP_BUG_EMAIL)<<endl;
			Qcout<<QObject::tr("%1 homepage: <%2>").arg(APP_SHORT_NAME).arg(APP_WEBSITTE)<<endl;
			Qcout<<QObject::tr("General help using GNU software: <http://www.gnu.org/gethelp/>")<<endl;
			return 0;
		}
	}

    MainWindow mainWin(startState, exec_binary);
	app.setActivationWindow(&mainWin);
	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &mainWin, SLOT(messageReceived(const QString&)));
	result = app.exec();
	db.close();
	return result;
}
