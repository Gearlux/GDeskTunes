#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#include "client.h"

class RemoteClient : public Client
{
    Q_OBJECT

public:
    RemoteClient();
    ~RemoteClient();

    Q_INVOKABLE void onPlay();
    Q_INVOKABLE void onPrevious();
    Q_INVOKABLE void onNext();

public slots:
    void bonjourConnect(const QList<BonjourRecord> &records);

};

#endif // REMOTECLIENT_H
