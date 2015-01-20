#include "protocol.h"

#include <QTcpSocket>
#include <QBuffer>
#include <QTimer>
#include <QPixmap>

#define CONVERT(variable, val) \
    switch(val.type()) \
    { \
    case QVariant::String: \
        variable =  Q_ARG(QString, val.toString()); \
    break; \
    case QVariant::Int: \
        variable = Q_ARG(int, val.toInt()); \
    break; \
    case QVariant::Pixmap: \
        variable = Q_ARG(QPixmap, val.value<QPixmap>() ); \
    break; \
    default: \
        qWarning() << "Conversion error"; \
    }


Protocol::Protocol(QTcpSocket *socket, QObject *target) :
    target(0),
    socket(socket),
    block_size(0)
{
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readCommand()));

    if (target == 0)
        this->target = this;
    else
        this->target = target;
}

Protocol::~Protocol()
{
    socket->deleteLater();
    socket = 0;
}

void Protocol::invokeMethod(const char* method, const QVariant &arg1, const QVariant &arg2,
                            const QVariant &arg3, const QVariant &arg4, const QVariant &arg5,
                            const QVariant &arg6, const QVariant &arg7, const QVariant &arg8,
                            const QVariant &arg9)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << method;
    if (arg1.isValid())
        out << arg1;
    if (arg2.isValid())
        out << arg2;
    if (arg3.isValid())
        out << arg3;
    if (arg4.isValid())
        out << arg4;
    if (arg5.isValid())
        out << arg5;
    if (arg6.isValid())
        out << arg6;
    if (arg7.isValid())
        out << arg7;
    if (arg8.isValid())
        out << arg8;
    if (arg9.isValid())
        out << arg9;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if (socket->state() == QTcpSocket::ConnectedState)
    {
        qDebug() << "Sending " << block.size() << " bytes";
        socket->write(block);
    }
    else
    {
        qDebug() << "Did not send "  << block.size() << " bytes";
    }
}

void Protocol::readCommand()
{
    qDebug() << "Protocol::readCommand()";

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    qDebug() << "Bytes available:" << socket->bytesAvailable() << "current block_size: " << block_size;

    if (block_size == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> block_size;
    }

    qDebug() << "Bytes available:" << socket->bytesAvailable() << "current block_size: " << block_size;
    if (socket->bytesAvailable() < block_size)
    {
        qDebug() << "Waiting for more data";
        return;
    }

    QByteArray data = socket->read(block_size);
    if (data.size() != block_size)
    {
        qDebug() << "Not enough data read";
        return;
    }
    if (socket->bytesAvailable() > 0)
    {
        qDebug() << "More data available, read next command";
        QTimer::singleShot(0, this, SLOT(readCommand()));
    }
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    in.setDevice(&buffer);

    char *command;
    in >> command;

    QList<QVariant> arguments;

    while(buffer.pos() < block_size)
    {
        QVariant next_argument;
        in >> next_argument;
        arguments.append(next_argument);
    }

    block_size = 0;

    QGenericArgument val0(0);
    QGenericArgument val1;
    QGenericArgument val2;
    QGenericArgument val3;
    QGenericArgument val4;
    QGenericArgument val5;
    QGenericArgument val6;
    QGenericArgument val7;
    QGenericArgument val8;
    QGenericArgument val9;

    qDebug() << "commandReceived: " << command << " " << arguments;
    qDebug() << "target: " << target->objectName();

    if (arguments.size() > 0)
        CONVERT(val0, arguments.at(0));
    if (arguments.size() > 1)
        CONVERT(val1, arguments.at(1));
    if (arguments.size() > 2)
        CONVERT(val2, arguments.at(2));
    if (arguments.size() > 3)
        CONVERT(val3, arguments.at(3));
    if (arguments.size() > 4)
        CONVERT(val4, arguments.at(4));
    if (arguments.size() > 5)
        CONVERT(val5, arguments.at(5));
    if (arguments.size() > 6)
        CONVERT(val6, arguments.at(6));
    if (arguments.size() > 7)
        CONVERT(val7, arguments.at(7));
    if (arguments.size() > 8)
        CONVERT(val8, arguments.at(8));
    if (arguments.size() > 9)
        CONVERT(val9, arguments.at(9));

    // qDebug() << command << *(QString*)val0.data();

    QMetaObject::invokeMethod(target, command,  Qt::QueuedConnection, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);

    delete command;
}

