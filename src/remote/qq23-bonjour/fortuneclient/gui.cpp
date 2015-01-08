/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <QtNetwork>

#include <QLabel>
#include <QTreeWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QGridLayout>

#include "gui.h"
#include "bonjourservicebrowser.h"
#include "client.h"

Gui::Gui(QWidget *parent)
    : QDialog(parent)
{
    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << tr("Available Fortune Servers"));

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    connectButton->setEnabled(false);

    disconnectButton = new QPushButton(tr("Disconnect"));
    disconnectButton->setDefault(true);
    disconnectButton->setEnabled(false);

    getFortuneButton = new QPushButton(tr("Get Fortune"));
    getFortuneButton->setDefault(true);
    getFortuneButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(disconnectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(treeWidget, 0, 0, 2, 2);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Fortune Client"));
    treeWidget->setFocus();

    connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
    connect(getFortuneButton, SIGNAL(clicked()), this, SLOT(requestNewFortune()));
    connect(disconnectButton, SIGNAL(clicked()),  this, SLOT(onDisconnectClicked()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    client = new Client(this);

    connect(client, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SLOT(updateRecords(const QList<BonjourRecord> &)));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError,const QString &)),
            this, SLOT(displayError(QAbstractSocket::SocketError, const QString &)));

    connect(client, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void Gui::onConnected()
{
    getFortuneButton->setEnabled(true);
    disconnectButton->setEnabled(true);
}

void Gui::onDisconnected()
{
    getFortuneButton->setEnabled(false);
    disconnectButton->setEnabled(false);
}
void Gui::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

void Gui::requestNewFortune()
{
    getFortuneButton->setEnabled(false);

    qDebug() << "Gui::requestNewFortune()";
    client->invokeMethod("requestNewFortune");
}


void Gui::displayError(QAbstractSocket::SocketError socketError, const QString &error)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(error));
    }

    getFortuneButton->setEnabled(true);
}

void Gui::enableConnectButton()
{
    connectButton->setEnabled(treeWidget->invisibleRootItem()->childCount() != 0);
}

void Gui::updateRecords(const QList<BonjourRecord> &list)
{
    treeWidget->clear();
    foreach (BonjourRecord record, list) {
        QVariant variant;
        variant.setValue(record);
        QTreeWidgetItem *processItem = new QTreeWidgetItem(treeWidget,
                                                           QStringList() << record.serviceName);
        processItem->setData(0, Qt::UserRole, variant);
    }
    
    if (treeWidget->invisibleRootItem()->childCount() > 0) {
        treeWidget->invisibleRootItem()->child(0)->setSelected(true);
    }
    enableConnectButton();
}

void Gui::setFortune(const QString &nextFortune)
{
    qDebug() << "Gui::setFortune(" << nextFortune << ")";
    currentFortune = nextFortune;
    statusLabel->setText(currentFortune);
    getFortuneButton->setEnabled(true);
}

void Gui::onConnectClicked()
{
    if (client->isConnected())
        client->disconnect();

    QList<QTreeWidgetItem *> selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QTreeWidgetItem *item = selectedItems.at(0);
    QVariant variant = item->data(0, Qt::UserRole);

    client->connectToServer(variant.value<BonjourRecord>());
}

void Gui::onDisconnectClicked()
{
    client->close();
}
