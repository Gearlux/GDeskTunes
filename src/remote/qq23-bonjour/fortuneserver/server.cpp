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
#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <stdlib.h>

#include "server.h"
#include "bonjourserviceregister.h"

#include "protocol.h"

Server::Server(QWidget *parent)
    : QDialog(parent)
{
    setWindowModality(Qt::ApplicationModal);
    statusLabel = new QLabel;
    quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }

    statusLabel->setText(tr("The server is running on port %1.\n"
                            "Run the Fortune Client example now.")
                         .arg(tcpServer->serverPort()));

    fortunes << tr("You've been leading a dog's life. Stay off the furniture.")
             << tr("You've got to think about tomorrow.")
             << tr("You will be surprised by a loud noise.")
             << tr("You will feel hungry again in another hour.")
             << tr("You might have mail.")
             << tr("You cannot kill time without injuring eternity.")
             << tr("Computers are not intelligent. They only think they are.");

    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendFortune()));


    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    
    bonjourRegister = new BonjourServiceRegister(this);
    bonjourRegister->registerService(BonjourRecord(tr("Fortune Server on %1").arg(QHostInfo::localHostName()),
                                            QLatin1String("_trollfortune._tcp"), QString()),
                                     tcpServer->serverPort());
    setWindowTitle(tr("Fortune Server"));

    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(sendAutoFortune()));
    timer->start(2000);

}

void Server::sendAutoFortune()
{
    qDebug() << "Server::sendAutoFortune()";
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->send(fortunes.at(qrand() % fortunes.size()) + QDateTime::currentDateTime().toString());
    }
    connections_mutex.unlock();
}


void Server::sendFortune()
{
    qDebug() << "Server::sendFortune()";

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

    qDebug() << clientConnection;

    Protocol* protocol = new Protocol(clientConnection);

    connections_mutex.lock();

    connections.append(protocol);

    QObject::connect(clientConnection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    QObject::connect(protocol, SIGNAL(commandReceived(QString)), this, SLOT(readCommand(QString)));

    connections_mutex.unlock();
}

void Server::onDisconnected()
{
    qDebug() << "Server::onDisconnected()" << sender();

    QTcpSocket *tcp = dynamic_cast<QTcpSocket*>(sender());
    if (tcp == 0) return;

    connections_mutex.lock();

    for(int i=connections.size()-1; i>=0; --i)
    {
        Protocol *proto = connections.at(i);
        if (proto->getSocket() == sender())
        {
            qDebug() << "Remove " << i;
            connections.removeAt(i);
            delete proto;
        }
    }

    connections_mutex.unlock();
}

void Server::readCommand(const QString &command)
{
    qDebug() << "Server::readCommand()";
    Protocol* socket = dynamic_cast<Protocol*>(sender());

    if (socket == 0)
    {
        qDebug() << "Error casting";
    }

    qDebug() << command;

    socket->send(fortunes.at(qrand() % fortunes.size()));
}
