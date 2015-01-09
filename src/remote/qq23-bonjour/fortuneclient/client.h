#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

#include "bonjourrecord.h"
#include "../fortuneserver/protocol.h"
#include <QAbstractSocket>

class BonjourServiceBrowser;
class BonjourServiceResolver;
class QHostInfo;

class Client: public QObject, public ProtocolFactory
{
    Q_OBJECT

public:
    Client(QObject *parent);
    ~Client();

signals:
    void currentBonjourRecordsChanged(const QList<BonjourRecord> &);
    void error(QAbstractSocket::SocketError error, const QString &errorMessage);
    void disconnected();
    void connected();

public:
    void connectToServer(const BonjourRecord &record);
    bool isConnected();
    void close();

    void invokeMethod(const char* method,
                      const QVariant &arg1 = QVariant(),
                      const QVariant &arg2 = QVariant(),
                      const QVariant &arg3 = QVariant(),
                      const QVariant &arg4 = QVariant(),
                      const QVariant &arg5 = QVariant(),
                      const QVariant &arg6 = QVariant(),
                      const QVariant &arg7 = QVariant(),
                      const QVariant &arg8 = QVariant(),
                      const QVariant &arg9 = QVariant());

protected:
    virtual Protocol* createProtocol(QTcpSocket *socket);

private slots:
    void connectToServer(const QHostInfo &hostInfo, int);
    void onError(QAbstractSocket::SocketError socketError);

private:
    Protocol *protocol;
    BonjourServiceBrowser *bonjourBrowser;
    BonjourServiceResolver *bonjourResolver;
    QObject *target;

};

#endif // FORTUNECLIENT_H
