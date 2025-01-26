/* Copyright 2009-2014 e-soul.org. All rights reserved.
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

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QString>
#include <QSize>
#include <QIntValidator>

#include "PreferencesDialog.hh"
#include "Preferences.hh"

PreferencesDialog::PreferencesDialog(QWidget* parent, Preferences* preferences) : QDialog(parent)
{
	this->preferences = preferences;

	indexLabel = new QLabel(tr("Index file:"));
	indexLineEdit = new QLineEdit;
	indexLineEdit->setText(QString(preferences->getPreference(INDEX_PREF).c_str()));
	fieldSize = new QSize(270, 26);
	indexLineEdit->setFixedSize(*fieldSize);

	checkIntervalLabel = new QLabel(tr("Check interval:"));
	checkIntervalLabel->setToolTip(tr("in hours, between 2 and 999999"));
	checkIntervalLineEdit = new QLineEdit;
	checkIntervalLineEdit->setText(QString(preferences->getPreference(CHECK_INTERVAL_PREF).c_str()));
	checkIntervalLineEdit->setToolTip(tr("in hours, between 2 and 999999"));
	//FIXME leaked object
	QIntValidator* checkIntervalValidator = new QIntValidator(2, 999999, checkIntervalLineEdit);
	checkIntervalLineEdit->setValidator(checkIntervalValidator);

	ok = new QPushButton(tr("OK"));
	QObject::connect(ok, SIGNAL(clicked()), this, SLOT(onOk()));
	cancel = new QPushButton(tr("Cancel"));
	QObject::connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

	mainLayout = new QGridLayout;
	mainLayout->addWidget(indexLabel, 0, 0);
	mainLayout->addWidget(indexLineEdit, 0, 1);
	mainLayout->addWidget(checkIntervalLabel, 1, 0);
	mainLayout->addWidget(checkIntervalLineEdit, 1, 1);
	mainLayout->addWidget(ok, 2, 0);
	mainLayout->addWidget(cancel, 2, 1);
	setLayout(mainLayout);

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Preferences"));
}

PreferencesDialog::~PreferencesDialog()
{
	delete indexLabel;
	delete indexLineEdit;
	delete fieldSize;
	delete checkIntervalLabel;
	delete checkIntervalLineEdit;
	delete ok;
	delete cancel;
	delete mainLayout;
}

void PreferencesDialog::onOk()
{
	QString index = indexLineEdit->text();
	if(!index.isEmpty())
	{
		preferences->setPreference(INDEX_PREF, index.toStdString());
	}

	if(!checkIntervalLineEdit->hasAcceptableInput())
	{
		QMessageBox::critical(this, tr("Input error"), tr("The \"Check interval\" field does not contain acceptable input"));
		return;
	}
	QString checkInterval = checkIntervalLineEdit->text();
	if(!checkInterval.isEmpty())
	{
		preferences->setPreference(CHECK_INTERVAL_PREF, checkInterval.toStdString());
	}
	close();
}
