#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>

class QTcpSocket;

class Protocol : public QObject
{
    Q_OBJECT

public:
    Protocol(QTcpSocket *socket);
    ~Protocol();

signals:
    void commandReceived(const QString &command);

public:
    void send(const QString &data);
    QTcpSocket* getSocket() { return this->socket; }

private slots:
    void readCommand();

private:
    QTcpSocket *socket;
    quint16 block_size;
};

#endif // TCPSOCKET_H
