#include "remoteserver.h"

RemoteServer::RemoteServer() :
    Server()
{

}

RemoteServer::~RemoteServer()
{

}

void RemoteServer::onClientPlay()
{
    qDebug() << "RemoteServer::onClientPlay()";
    emit play();
}

void RemoteServer::onClientNext()
{
    qDebug() << "RemoteServer::onClientNext()";
    emit next();
}

void RemoteServer::onClientPrevious()
{
    qDebug() << "RemoteServer::onClientPrevious()";
    emit previous();
}

