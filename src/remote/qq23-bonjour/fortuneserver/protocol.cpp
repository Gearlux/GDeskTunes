#include "protocol.h"

#include <QTcpSocket>
#include <QBuffer>

Protocol::Protocol(QTcpSocket *socket) :
    socket(socket),
    block_size(0)
{
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readCommand()));
}

Protocol::~Protocol()
{
    socket->deleteLater();
    socket = 0;
}

void Protocol::send(const QString &data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << data;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if (socket->state() == QTcpSocket::ConnectedState)
        socket->write(block);
    else
    {
        qDebug() << "Did not write " << data;
    }
}

void Protocol::readCommand()
{
    qDebug() << "Protocol::readCommand()";

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    qDebug() << socket->bytesAvailable() << block_size;

    if (block_size == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> block_size;
    }

    if (socket->bytesAvailable() < block_size)
        return;

    QByteArray data = socket->read(block_size);
    if (data.size() != block_size)
    {
        qDebug() << "Not enough data read";
        return;
    }
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    in.setDevice(&buffer);
    QString command;
    in >> command;

    block_size = 0;

    emit commandReceived(command);
}
