#include "client.h"

#include "protocol.h"

#include <QTcpSocket>
#include <QHostInfo>
#include <QDateTime>

#include "bonjourservicebrowser.h"
#include "bonjourserviceresolver.h"

Client::Client(QObject *parent) :
    QObject(parent),
    protocol(0),
    bonjourResolver(0)
{
    BonjourServiceBrowser *bonjourBrowser = new BonjourServiceBrowser(this);

    connect(bonjourBrowser, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)));

    bonjourBrowser->browseForServiceType(QLatin1String("_trollfortune._tcp"));
}

Client::~Client()
{

}

void Client::connectToServer(const QHostInfo &hostInfo, int port)
{
    qDebug() << "Client::connectToServer()";

    const QList<QHostAddress> &addresses = hostInfo.addresses();
    if (!addresses.isEmpty())
    {
        if (isConnected())
        {
            qWarning() << "Still connnected";
        }
        else
        {
            qDebug() << "connectToHost";
            protocol->getSocket()->connectToHost(addresses.first(), port);

            emit connected();
        }
    }
}

void Client::onError(QAbstractSocket::SocketError socketError)
{
    emit error(socketError, protocol->getSocket()->errorString());
}

void Client::close()
{
    if (isConnected())
    {
        protocol->getSocket()->abort();

        emit disconnected();
    }
}

void Client::invokeMethod(const char* method, const QVariant &arg1, const QVariant &arg2,
                          const QVariant &arg3, const QVariant &arg4, const QVariant &arg5,
                          const QVariant &arg6, const QVariant &arg7, const QVariant &arg8,
                          const QVariant &arg9)
{
    if (!isConnected())
    {
        qWarning() << "Not connected";
        return;
    }

    protocol->invokeMethod(method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

bool Client::isConnected()
{
    if (protocol == 0) return false;
    return protocol->getSocket()->state() == QTcpSocket::ConnectedState;
}

void Client::connectToServer(const BonjourRecord &record)
{
    if (protocol != 0)
        protocol->getSocket()->abort();

    if (!bonjourResolver) {
        bonjourResolver = new BonjourServiceResolver(this);
        connect(bonjourResolver, SIGNAL(bonjourRecordResolved(const QHostInfo &, int)),
                this, SLOT(connectToServer(const QHostInfo &, int)));

        QTcpSocket *tcpSocket = new QTcpSocket(this);
        connect(tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
        connect(tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));

        connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(onError(QAbstractSocket::SocketError)));

        qDebug() << "Creating protocol";
        protocol = createProtocol(tcpSocket);
    }
    bonjourResolver->resolveBonjourRecord(record);
}

Protocol* Client::createProtocol(QTcpSocket *socket)
{
    return new Protocol(socket, this);
}
