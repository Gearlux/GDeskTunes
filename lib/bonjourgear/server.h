#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "protocol.h"

// Forward declarations
class BonjourServiceRegister;

class Server : public QTcpServer, public ProtocolFactory
{
    Q_OBJECT

public:
    Server(QLatin1String service, QObject *parent = 0);
    ~Server();

public:
    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);

protected:
    virtual Protocol* createProtocol(QTcpSocket *socket);

private slots:
    void onDisconnected();
    void onNewConnection();

protected:
    QMutex connections_mutex;
    QList<Protocol*> connections;

private:
    BonjourServiceRegister *bonjourRegister;
    QLatin1String service;
};

#endif // SERVER_H
