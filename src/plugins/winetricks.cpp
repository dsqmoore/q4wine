/***************************************************************************
 *   Copyright (C) 2009 by Pavel Zinin (pashazz)                              *
 *   pzinin@gmail.com                                                     *
 *   Copyright (C) 2010 by Malakhov Alexey                           *
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

#include "winetricks.h"

winetricks::winetricks(QString prefixName, QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	// Loading libq4wine-core.so
	libq4wine.setFileName("libq4wine-core");

	if (!libq4wine.load()){
		libq4wine.load();
	}

	// Getting corelib calss pointer
	CoreLibClassPointer = (CoreLibPrototype *) libq4wine.resolve("createCoreLib");
	CoreLib.reset((corelib *)CoreLibClassPointer(true));

	this->winetricks_bin.append(QDir::homePath());
	this->winetricks_bin.append("/.config/");
	this->winetricks_bin.append(APP_SHORT_NAME);
	this->winetricks_bin.append("/winetricks");

	this->prefix_path = db_prefix.getPath(prefixName);
	this->console_bin = CoreLib->getSetting("console", "bin").toString();
	this->console_args = CoreLib->getSetting("console", "args", FALSE).toString();

	setupUi(this);

	connect (cmdInstall, SIGNAL (clicked()), this, SLOT(run_winetricks()));
	connect (cmdExit, SIGNAL (clicked()), this, SLOT(accept()));
	connect (cmdInstWinetricks, SIGNAL (clicked()), this, SLOT (install_winetricks()));

	parse();
	//fill list
}

void winetricks::install_winetricks() {
	downloadwinetricks ();
        parse();
}

void winetricks::run_winetricks(){

    if (!lstMain->currentItem()){
        this->parse();
        return;
    }

#ifdef DEBUG
    qDebug()<<"[plugin] run item";
#endif

    QStringList args;
	if (!console_args.isEmpty()){
		// If we have any conslope parametres, we gona preccess them one by one
		QStringList cons_args = console_args.split(" ");
		for (int i=0; i<cons_args.count(); i++){
			if (!cons_args.at(i).isEmpty())
				args.append(cons_args.at(i));
		}
	}

	args.append(CoreLib->getSetting("system", "sh").toString());
	args.append("-c");

	QString arg;
		if (!CoreLib->getSetting("network", "host", false).toString().isEmpty()){
			arg.append("http_proxy=\"http://");
			arg.append(CoreLib->getSetting("network", "host", false).toString());
			if (!CoreLib->getSetting("network", "port", false).toString().isEmpty()){
				arg.append(":");
				arg.append(CoreLib->getSetting("network", "port", false).toString());
			}
			arg.append("\" ftp_proxy=\"http://");
			arg.append(CoreLib->getSetting("network", "host", false).toString());
			if (!CoreLib->getSetting("network", "port", false).toString().isEmpty()){
				arg.append(":");
				arg.append(CoreLib->getSetting("network", "port", false).toString());
			}
			arg.append("\" ");
		}

		arg.append("WINEPREFIX=");
		arg.append(this->prefix_path);
		arg.append(" ");
		arg.append(this->winetricks_bin);
		arg.append(" ");
		arg.append(lstMain->currentItem()->text());

	args.append(arg);

    QProcess proc(this);
    proc.startDetached(console_bin, args, QDir::homePath());
	return;
}

void winetricks::downloadwinetricks () {
		/*
		 * Downloading winetriks and installing it
		 */

	QStringList args;
	if (!console_args.isEmpty()){
		// If we have any conslope parametres, we gona preccess them one by one
		QStringList cons_args = console_args.split(" ");
		for (int i=0; i<cons_args.count(); i++){
			if (!cons_args.at(i).isEmpty())
				args.append(cons_args.at(i));
		}
	}


	args.append(CoreLib->getSetting("system", "sh").toString());
	args.append("-c");
	QString arg;

		if (!CoreLib->getSetting("network", "host", false).toString().isEmpty()){
			arg.append("http_proxy=\"http://");
			arg.append(CoreLib->getSetting("network", "host", false).toString());
			if (!CoreLib->getSetting("network", "port", false).toString().isEmpty()){
				arg.append(":");
				arg.append(CoreLib->getSetting("network", "port", false).toString());
			}
			arg.append("\" ftp_proxy=\"http://");
			arg.append(CoreLib->getSetting("network", "host", false).toString());
			if (!CoreLib->getSetting("network", "port", false).toString().isEmpty()){
				arg.append(":");
				arg.append(CoreLib->getSetting("network", "port", false).toString());
			}
			arg.append("\" ");
		}

		arg.append(CoreLib->getWhichOut("wget"));
		arg.append(" http://kegel.com/wine/winetricks -O ");
		arg.append(this->winetricks_bin);
		arg.append(" && ");
		arg.append(CoreLib->getWhichOut("chmod"));
		arg.append(" +x ");
		arg.append(this->winetricks_bin);

	args.append(arg);

    Process exportProcess(args, console_bin, QDir::homePath(), tr("Downloading and installing winetricks..."), tr("Plz wait..."));

		exportProcess.exec();
		// setting help

#ifdef DEBUG
    qDebug()<<"[plugin] download done";
#endif
    return;


}



/*
Not needed right now...
void winetricks::changeEvent(QEvent *e)
{
	switch (e->type()) {
		case QEvent::LanguageChange:
			retranslateUi(this);
		break;
		default:
		break;
	}
}
*/

void winetricks::addToList(QString arg) {
#ifdef DEBUG
    qDebug()<<"[plugin] add to list";
#endif
QStringList args = arg.split(" ");
QString name = args.at(0);
int pos = arg.count() - name.count();
QString desc = arg.right(pos);
names.append(name);
descs.append(desc);
#ifdef DEBUG
    qDebug()<<"[plugin] add to list end";
#endif
}


void winetricks::parse() {

#ifdef DEBUG
    qDebug()<<"[plugin] parsing winetriks output";
#endif

    if (!QFile(this->winetricks_bin).exists()){
            QMessageBox::warning(this, tr("Error"), tr("<p>q4wine can't locate winetricks at %1 path!</p><p>The script is maintained and hosted by DanKegel at http://www.kegel.com/wine/winetricks.  You can get it from the commandline with the command:</p><p>wget http://www.kegel.com/wine/winetricks</p><p>Or use \"Install winetricks\" button.</p>").arg(this->winetricks_bin));
            return;
    }

	//create a Winetricks process
	  QProcess p(this);
	QString pargs;
	pargs.append(winetricks_bin);
	pargs.append(" --kegelfix");

   p.start(pargs);

   p.waitForFinished();
	 //get output
   QString lang = CoreLib->getLocale();
#ifdef DEBUG
   qDebug()<<lang;
#endif

   /* Getting list */
  QTextCodec *codec = QTextCodec::codecForName(lang.toAscii());
  QString wOut = codec->toUnicode(p.readAllStandardOutput());

	//parse now
bool isNowParse = false;
QStringList strs = wOut.split("\n");
QString str;

foreach (str, strs){
	if (!str.isEmpty()){
		str = str.trimmed();

		if (str == "Apps:" || str == "Pseudopackages:"){continue;}
		if (str == "Packages:") {isNowParse = true;
			continue;
		}
		if (isNowParse) {addToList(str);}
	}
}

#ifdef DEBUG
    qDebug()<<"[plugin] parsing winetriks output done";
#endif


str.clear();
foreach (str, names) {
        lstMain->addItem(str);
}

#ifdef DEBUG
    qDebug()<<"[plugin] add to list done";
#endif

}




void winetricks::on_lstMain_itemClicked(QListWidgetItem* item)
{

	int i;
  for (i=0; i < descs.count() -1; ++i) {
   if (names.at(i) ==  item->text()){
           label->setText (descs.at(i).trimmed());
   }
  }
}


