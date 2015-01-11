#ifndef REMOTESERVER_H
#define REMOTESERVER_H

#include "server.h"

class RemoteServer : public Server
{
    Q_OBJECT

public:
    RemoteServer();
    ~RemoteServer();

signals:
    void play();
    void next();
    void previous();

public slots:
    void onClientPlay();
    void onClientNext();
    void onClientPrevious();
};

#endif // REMOTESERVER_H
