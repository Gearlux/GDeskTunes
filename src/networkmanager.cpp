#include "networkmanager.h"
#include <QNetworkRequest>
#include <QtGlobal>
#include <QFile>
#include <QTimer>

ImageReply::ImageReply(const QUrl &url) :
    QNetworkReply(),
    offset(0)
{
    setUrl(url);

    QString resourceName = url.toString().replace("https", "").replace("http","").replace("//","/");
    qDebug() << resourceName;
     setUrl(QUrl(resourceName));

    QFile file(resourceName);
    if (file.exists())
    {
        file.open(ReadOnly);
        content = file.readAll();
        file.close();
        qDebug() << "We have read " << content.size();
    }
    else
        qDebug() << "File does not exist";

    open(ReadOnly);
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(content.size()));
    QTimer::singleShot(0, this, SLOT(deferEmitSignals()));
}

void ImageReply::deferEmitSignals()
{
    emit readyRead();
    emit finished();
}

void ImageReply::abort()
{
    qDebug() << "ImageReply::abort()";
}

qint64 ImageReply::bytesAvailable() const
{
    int result = content.size() - offset + QIODevice::bytesAvailable();
    qDebug() << "ImageReply::bytesAvailable()" << result;
   return result;
}

bool ImageReply::isSequential() const
{
    return true;
}

qint64 ImageReply::readData(char *data, qint64 maxSize)
{
    qDebug() << "ImageReply::readData(" << (void*)data << "," << maxSize << ")";
    if (offset < content.size()) {
        qint64 number = qMin(maxSize, content.size() - offset);
        memcpy(data, content.constData() + offset, number);
        offset += number;
        return number;
    } else
        return -1;
}

QByteArray ImageReply::read(qint64 maxSize)
{
    qDebug() << "ImageReply::read(" << maxSize << ")";
    return QByteArray();
}

QByteArray ImageReply::readAll()
{
    qDebug() << "ImageReply::readAll()";
    return content;
}


NetworkManager::NetworkManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

QNetworkReply *NetworkManager::createRequest(
    QNetworkAccessManager::Operation operation, const QNetworkRequest &request,
    QIODevice *device)
{
    if (request.url().host() != "radiant-player-mac")
        return QNetworkAccessManager::createRequest(operation, request, device);

    if (operation == GetOperation)
    {
        qDebug() << "Requesting image" << request.url();
        return new ImageReply(request.url());
    }
    else
        return QNetworkAccessManager::createRequest(operation, request, device);
}
