#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QVariant>

class QTcpSocket;

class ProtocolExecutor : public QObject
{
    Q_OBJECT

public:
    ProtocolExecutor(QObject *target, char* command, QList<QVariant> arguments);

public slots:
    void execute();

private:
    QObject *target;
    char* command;
    QList<QVariant> arguments;
};

class Protocol : public QObject
{
    Q_OBJECT

public:
    Protocol(QTcpSocket *socket, QObject *target);
    ~Protocol();

public:
    void invokeMethod(const char* method,
                      QVariant arg1 = QVariant(),
                      QVariant arg2 = QVariant(),
                      QVariant arg3 = QVariant(),
                      QVariant arg4 = QVariant(),
                      QVariant arg5 = QVariant(),
                      QVariant arg6 = QVariant(),
                      QVariant arg7 = QVariant(),
                      QVariant arg8 = QVariant(),
                      QVariant arg9 = QVariant());

    QTcpSocket* getSocket() { return this->socket; }

    void parse(QDataStream in, int size, QByteArray data);
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
