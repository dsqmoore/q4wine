/***************************************************************************
 *   Copyright (C) 2009 by Pavel Zinin (pashazz)                              *
 *   pzinin@gmail.com                                                     *
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
 *   In addition, as a special exception, the copyright holders give       *
 *   permission to link the code of this program with any edition of       *
 *   the Qt library by Trolltech AS, Norway (or with modified versions     *
 *   of Qt that use the same license as Qt), and distribute linked         *
 *   combinations including the two.  You must obey the GNU General        *
 *   Public License in all respects for all of the code used other than    *
 *   Qt.  If you modify this file, you may extend this exception to        *
 *   your version of the file, but you are not obligated to do so.  If     *
 *   you do not wish to do so, delete this exception statement from        *
 *   your version.                                                         *
 ***************************************************************************/

#include "winetricks.h"
#include "ui_winetricks.h"

 winetricks::winetricks() :
         _prefix ("~/.wine"),
    m_ui(new Ui::winetricks)
 {
    m_ui->setupUi(this);
    core = new CoreMethods();
    _winetricks = core->getWhichOut("winetricks");
    //console
    _console = setConsole ();
    //button connections
connect (m_ui->buttonBox, SIGNAL (accepted()), this, SLOT(onaccept()));
connect (m_ui->buttonBox, SIGNAL (rejected()), this, SLOT(onreject()));
connect (m_ui->btnInstWinetricks, SIGNAL (clicked()), this, SLOT (instwinetricks()));

}


winetricks::~winetricks()
{
    delete m_ui;
    delete core;
}

QString winetricks::setConsole() { //console search
QString _console; //to return
    //fist, konsole
    if (!core->getWhichOut("konsole").isEmpty())
{
        _console = core->getWhichOut("konsole");
                   return _console;
               }
    //then gnome-terminal
    if (!core->getWhichOut("gnome-terminal").isEmpty())  {
        _console = core->getWhichOut("gnome-terminal");
        return _console;
    }
    //then xfce4-terminal
    if (!core->getWhichOut("xfce4-terminal").isEmpty()) {
        _console = core->getWhichOut("xfce4-terminal");
        return _console;
    }
    //then xterm
    if (!core->getWhichOut("xterm").isEmpty()) {
        _console = core->getWhichOut("xterm");
        return _console;
    }
}
void winetricks::setPrefix (QString prefix) {
    _prefix = prefix;
    }
void winetricks::instwinetricks() {install_winetricks();}
void winetricks::install_winetricks() {
QMessageBox msg;
msg.setText(tr("Not implemented. Manually download winetricks from the http://kegel.com/wine/winetricks, copy it to /usr/bin, and set execution permissions"));
msg.setWindowTitle(tr("Q4Wine"));
msg.exec();
}

void winetricks::run_winetricks(){
 QObject *parent;
 QStringList args;

       QString program = _console + " -e " + _winetricks + " "  + m_ui->lstMain->currentItem()->text() ;
       ;

 QProcess *swinetricks = new QProcess (parent);
swinetricks->start (program);
}



void winetricks::onaccept() {
run_winetricks();
this->close();
}
void winetricks::onreject() { this->close();}
void winetricks::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}