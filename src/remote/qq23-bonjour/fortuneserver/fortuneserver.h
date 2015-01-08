#ifndef FORTUNESERVER_H
#define FORTUNESERVER_H

#include "server.h"

class FortuneServer;

class FortuneProtocol : public Protocol
{
    Q_OBJECT

public:
    FortuneProtocol(QTcpSocket *socket, FortuneServer *server);

public slots:
    void requestNewFortune();

private:
    FortuneServer *server;
};


class FortuneServer : public Server
{
    friend class FortuneProtocol;

    Q_OBJECT

public:
    FortuneServer(QObject *parent = 0);
    ~FortuneServer();

public slots:
    void sendRandomFortune();

protected:
    virtual Protocol* createProtocol(QTcpSocket *socket);

private:
    QStringList fortunes;
};

#endif // FORTUNESERVER_H
