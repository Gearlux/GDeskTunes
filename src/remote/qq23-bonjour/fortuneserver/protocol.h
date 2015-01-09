#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QVariant>

class QTcpSocket;

class Protocol : public QObject
{
    Q_OBJECT

public:
    Protocol(QTcpSocket *socket, QObject *target);
    ~Protocol();

public:
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

    QTcpSocket* getSocket() { return this->socket; }

private slots:
    void readCommand();

protected:
    QObject *target;

private:
    QTcpSocket *socket;
    quint16 block_size;
};

class ProtocolFactory
{
protected:
    virtual Protocol* createProtocol(QTcpSocket *socket) = 0;
};

#endif // TCPSOCKET_H
