/* Copyright 2009-2025 e-soul.org. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 *      the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *      and the following disclaimer in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY E-SOUL.ORG ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL E-SOUL.ORG
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the authors and
 * should not be interpreted as representing official policies, either expressed or implied, of e-soul.org.
 */

#include <sstream>

#include <QSystemTrayIcon>
#include <QTimer>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QApplication>
#include <QMenu>

#include "Trac.hh"
#include "PortChecker.hh"
#include "PreferencesDialog.hh"

Trac::Trac()
{
	// initialization
	oldList = NULL;
	model = new QStringListModel;
	prefs = new Preferences;
	prefs->load();

	// UI initialization
	checkNowButton = new QPushButton(tr("Check Now"));
	QObject::connect(checkNowButton, SIGNAL(clicked()), this, SLOT(onCheckNow()));

	QListView* listView = new QListView;
	listView->setModel(model);

	QLabel* listLabel = new QLabel(tr("There are updates for the following ports:"));
	totalLabel = new QLabel(tr("Total: "));

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(checkNowButton);
	layout->addWidget(listLabel);
	layout->addWidget(listView);
	layout->addWidget(totalLabel);

	QWidget* widget = new QWidget;
	setCentralWidget(widget);
	widget->setLayout(layout);

	QMenu* programmeMenu = createProgrammeMenu();
	initMenuBar(programmeMenu);
	initTrayIcon(programmeMenu);
	initTimer();
}

Trac::~Trac()
{
}

void Trac::onCheckNow()
{
	check();
}

void Trac::check()
{
	timer->stop();
	checkNowButton->setEnabled(false);
	checkNowButton->setText(tr("Checking, please wait..."));
	//FIXME is a new port checker really needed?
	pc = new PortChecker(prefs);
	QObject::connect(pc, SIGNAL(finished()), this, SLOT(getList()));
	pc->start(QThread::NormalPriority);
}

void Trac::exitProgramme()
{
	prefs->persist();
	QApplication::quit();
}

void Trac::closeEvent(QCloseEvent *event)
{
	hide();
	event->ignore();
}

void Trac::getList()
{
	QStringList* list = pc->getList();
	delete pc;
	model->setStringList(*list);
	bool newList = false;
	if(oldList != NULL)
	{
		newList = !equal(list, oldList);
		delete oldList;
	}
	else
	{
		newList = true;
	}
	oldList = list;
	if(newList && !list->isEmpty())
	{
		trayIcon->showMessage(tr("Port updates"), tr("A new list of port updates is available."));
		//FIXME remove hardcoded path
		trayIcon->setIcon(QIcon(tr("/usr/local/share/portrac/updates-available.png")));
		trayIcon->setToolTip(tr("Portrac: updates available"));
	}
	else if(list->isEmpty())
	{
		//FIXME remove hardcoded path
		trayIcon->setIcon(QIcon(tr("/usr/local/share/portrac/up-to-date.png")));
		trayIcon->setToolTip(tr("Portrac: up to date"));
	}
	timer->start(determineCheckInterval());
	checkNowButton->setEnabled(true);
	checkNowButton->setText(tr("Check Now"));

	updateTotalLabel(list);
}

void Trac::preferences()
{
	PreferencesDialog* pd = new PreferencesDialog(this, prefs);
	pd->show();
	/*
	this dialog sets Qt::WA_DeleteOnClose
	therefore no delete call needed
	*/
}

bool Trac::equal(QStringList* list1, QStringList* list2)
{
	if(list1->size() == list2->size())
	{
		for(int i = 0; i < list1->size(); i++)
		{
			if(list1->at(i).compare(list2->at(i)) != 0)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

int Trac::determineCheckInterval()
{
	int multiplier = atoi(prefs->getPreference(CHECK_INTERVAL_PREF).c_str());
	return DEFAULT_TIMER_INTERVAL * multiplier;
}

void Trac::updateTotalLabel(QStringList* list)
{
	ostringstream oss("Total: ", ios_base::app);
	oss << list->size();
	QString tl(oss.str().c_str());
	totalLabel->setText(tl);
}

QMenu* Trac::createProgrammeMenu()
{
	QAction* preferencesAct = new QAction(tr("&Preferences"), this);
	preferencesAct->setStatusTip(tr("Choose your preferences."));
	QObject::connect(preferencesAct, SIGNAL(triggered()), this, SLOT(preferences()));

	QAction* exitAct = new QAction(tr("&Exit"), this);
	exitAct->setStatusTip(tr("Exit this programme."));
	QObject::connect(exitAct, SIGNAL(triggered()), this, SLOT(exitProgramme()));

	QMenu* programmeMenu = new QMenu(tr("&Programme"), menuBar());
	programmeMenu->addAction(preferencesAct);
	programmeMenu->addAction(exitAct);

	return programmeMenu;
}

void Trac::initMenuBar(QMenu* programmeMenu)
{
	menuBar()->addMenu(programmeMenu);
}

void Trac::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Context)
	{
		trayIcon->contextMenu()->exec();
	}
	else
	{
		if(!isVisible()) {
			show();
			setWindowState(Qt::WindowActive);
		} else {
			hide();
		}
	}
}

void Trac::initTrayIcon(QMenu* programmeMenu)
{
	trayIcon = new QSystemTrayIcon(this);
	//FIXME remove hardcoded path
	trayIcon->setIcon(QIcon(tr("/usr/local/share/portrac/portrac.png")));
	trayIcon->setToolTip(tr("Portrac"));
	trayIcon->setContextMenu(programmeMenu);
	trayIcon->show();
	QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	QObject::connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(show()));
}

void Trac::initTimer()
{
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(check()));
	timer->start(determineCheckInterval());
}
