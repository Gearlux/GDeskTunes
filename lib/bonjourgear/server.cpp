#include "server.h"

#include "protocol.h"

#include <QtNetwork>

#include "bonjourserviceregister.h"

Server::Server(QObject *parent):
    QTcpServer(parent),
    bonjourRegister(0)
{

}

Server::~Server()
{

}

bool Server::listen(const QHostAddress &address, quint16 port)
{
    if (bonjourRegister != 0)
        return false;

    bool result = QTcpServer::listen(address, port);

    if (result)
    {
        bonjourRegister = new BonjourServiceRegister(this);
        bonjourRegister->registerService(BonjourRecord(tr("Fortune Server on %1").arg(QHostInfo::localHostName()),
                                                QLatin1String("_trollfortune._tcp"), QString()),
                                         this->serverPort());
    }

    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    return result;
}

Protocol* Server::createProtocol(QTcpSocket *socket)
{
    return new Protocol(socket, this);
}

void Server::onNewConnection()
{
    QTcpSocket *clientConnection = nextPendingConnection();

    qDebug() << clientConnection;

    Protocol* protocol = createProtocol(clientConnection);

    connections_mutex.lock();

    connections.append(protocol);

    QObject::connect(clientConnection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

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
