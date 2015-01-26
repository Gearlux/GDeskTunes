#include "protocol.h"

#include <QTcpSocket>
#include <QBuffer>
#include <QTimer>
#include <QPixmap>

#define CONVERT(variable, val) \
{ \
    switch(val.type()) \
{ \
    case QVariant::String: \
    variable = new Q_ARG(QString, val.toString()); \
    break; \
    case QVariant::Int: \
    variable = new Q_ARG(int, val.toInt()); \
    break; \
    case QVariant::Pixmap: \
    variable = new Q_ARG(QPixmap, val.value<QPixmap>() ); \
    break; \
    default: \
    qWarning() << "Conversion error"; \
    } \
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

void Protocol::invokeMethod(const char* method, QVariant arg1, QVariant arg2,
                            QVariant arg3, QVariant arg4, QVariant arg5,
                            QVariant arg6, QVariant arg7, QVariant arg8,
                            QVariant arg9)
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

    int size = block_size;
    block_size = 0;

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    in.setDevice(&buffer);

    char *command;
    QList<QVariant> arguments;

    in >> command;

    qDebug() << "command:" << command;

    while(buffer.pos() < size)
    {
        QVariant next_argument;
        in >> next_argument;
        arguments.append(next_argument);
    }

    qDebug() << "arguments: " << arguments;

    ProtocolExecutor* executor = new ProtocolExecutor(target, command, arguments);

    if (socket->bytesAvailable() > 0)
    {
        qDebug() << "More data available, read next command";
        QTimer::singleShot(0, this, SLOT(readCommand()));
    }

    QTimer::singleShot(0, executor, SLOT(execute()));

    // parse(in, size, data);
}


ProtocolExecutor::ProtocolExecutor(QObject *target, char* command, QList<QVariant> arguments) :
    target(target),
    command(command),
    arguments(arguments)
{
}

void ProtocolExecutor::execute()
{
    qDebug() << "ProtocolExecutor::execute()";

    QGenericArgument* val0 = 0;
    QGenericArgument* val1 = 0;
    QGenericArgument* val2 = 0;
    QGenericArgument* val3 = 0;
    QGenericArgument* val4 = 0;
    QGenericArgument* val5 = 0;
    QGenericArgument* val6 = 0;
    QGenericArgument* val7 = 0;
    QGenericArgument* val8 = 0;
    QGenericArgument* val9 = 0;

    qDebug() << "commandReceived: " << command << " " << arguments;
    qDebug() << "target: " << target->objectName();

    if (arguments.size() > 0)
    {
        CONVERT(val0, arguments.at(0));
    }
    else val0 = new QGenericArgument();
    if (arguments.size() > 1)
    {
        CONVERT(val1, arguments.at(1));
    }
    else val1 = new QGenericArgument();
    if (arguments.size() > 2)
    {
        CONVERT(val2, arguments.at(2));
    }
    else val2 = new QGenericArgument();
    if (arguments.size() > 3)
    {
        CONVERT(val3, arguments.at(3));
    }
    else val3 = new QGenericArgument();
    if (arguments.size() > 4)
    {
        CONVERT(val4, arguments.at(4));
    }
    else val4 = new QGenericArgument();
    if (arguments.size() > 5)
    {
        CONVERT(val5, arguments.at(5));
    }
    else val5 = new QGenericArgument();
    if (arguments.size() > 6)
    {
        CONVERT(val6, arguments.at(6));
    }
    else val6 = new QGenericArgument();
    if (arguments.size() > 7)
    {
        CONVERT(val7, arguments.at(7));
    }
    else val7 = new QGenericArgument();
    if (arguments.size() > 8)
    {
        CONVERT(val8, arguments.at(8));
    }
    else val8 = new QGenericArgument();
    if (arguments.size() > 9)
    {
        CONVERT(val9, arguments.at(9));
    }
    else
        val9 = new QGenericArgument();

    QMetaObject::invokeMethod(target, command,  Qt::DirectConnection, *val0, *val1, *val2, *val3, *val4, *val5, *val6, *val7, *val8, *val9);

    delete command;

    delete val0;
    delete val1;
    delete val2;
    delete val3;
    delete val4;
    delete val5;
    delete val6;
    delete val7;
    delete val8;
    delete val9;

    this->deleteLater();
}
