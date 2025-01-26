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

#ifndef TRAC_HH
#define TRAC_HH

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QPushButton>
#include <QLabel>

#include "PortChecker.hh"
#include "Preferences.hh"

#define DEFAULT_TIMER_INTERVAL 60 * 60 * 1000

class Trac : public QMainWindow
{
	Q_OBJECT

	public:
		/*
		 * Constructor.
		 */
		Trac();
		/*
		 * Destructor.
		 */
		~Trac();

	public slots:
		/*
		 * Called when the Check Now button is pressed.
		 */
		void onCheckNow();
		/*
		 * Checks for port updates.
		 */
		void check();
		/*
		 * Terminates the applicatoin.
		 */
		void exitProgramme();
		/*
		 * Gets the list of port updates and
		 * displays it accordingly.
		 */
		void getList();
		/*
		 * Brings up the preferences dialog.
		 */
		void preferences();
		/*
		 * Called when the tray icon is clicked:
		 * if it's left-cliked or double-clicked
		 * the main window is either minimized or maximized
		 * if it's right-clicked the context menu is shown.
		 */
		void trayActivated(QSystemTrayIcon::ActivationReason);

	protected:
		/*
		 * Called when the window close button is called.
		 * This implementation ignores the close event and
		 * minimizes the window.
		 */
		void closeEvent(QCloseEvent *event);

	private:
		QPushButton* checkNowButton;
		QStringList* oldList;
		QStringListModel* model;
		QSystemTrayIcon* trayIcon;
		QTimer* timer;
		QLabel* totalLabel;
		PortChecker* pc;
		Preferences* prefs;

		bool equal(QStringList* list1, QStringList* list2);
		int determineCheckInterval();
		void updateTotalLabel(QStringList* list);

		QMenu* createProgrammeMenu();
		void initMenuBar(QMenu* programmeMenu);
		void initTrayIcon(QMenu* programmeMenu);
		void initTimer();

	signals:
		void listChanged();
};

#endif // TRAC_HH
