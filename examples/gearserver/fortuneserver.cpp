#include <QDateTime>

#include "fortuneserver.h"

FortuneServer::FortuneServer(QObject *parent) : Server(QLatin1String("_trollfortune._tcp"), parent)
{
    fortunes << tr("You've been leading a dog's life. Stay off the furniture.")
             << tr("You've got to think about tomorrow.")
             << tr("You will be surprised by a loud noise.")
             << tr("You will feel hungry again in another hour.")
             << tr("You might have mail.")
             << tr("You cannot kill time without injuring eternity.")
             << tr("Computers are not intelligent. They only think they are.");
}

FortuneServer::~FortuneServer()
{

}

void FortuneServer::sendRandomFortune()
{
    QString fortune = fortunes.at(qrand() % fortunes.size()) + QDateTime::currentDateTime().toString();
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("setFortune", fortune);
    }
    connections_mutex.unlock();
}

Protocol* FortuneServer::createProtocol(QTcpSocket *socket)
{
    return new FortuneProtocol(socket, this);
}


FortuneProtocol::FortuneProtocol(QTcpSocket *socket, FortuneServer *server) :
    Protocol(socket, 0),
    server(server)
{

}

void FortuneProtocol::requestNewFortune()
{
    qDebug() << "Server::requestNewFortune()";
    invokeMethod("setFortune", server->fortunes.at(qrand() % server->fortunes.size()));
}
