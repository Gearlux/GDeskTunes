#include "remoteclient.h"

RemoteClient::RemoteClient() :
    Client(0)
{
    connect(this, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SLOT(bonjourConnect(const QList<BonjourRecord> &)));
}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::bonjourConnect(const QList<BonjourRecord> &records)
{
    if (records.size() == 0)
        return;

    qDebug() << "bonjourConnect";
    BonjourRecord first = records.at(0);
    connectToServer(first);
}

void RemoteClient::onPlay()
{
    invokeMethod("onClientPlay");
}

void RemoteClient::onPrevious()
{
    invokeMethod("onClientPrevious");
}

void RemoteClient::onNext()
{
    invokeMethod("onClientNext");
}
