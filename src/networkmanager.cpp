// #define QT_NO_DEBUG_OUTPUT

#include "networkmanager.h"
#include <QNetworkRequest>
#include <QtGlobal>
#include <QFile>
#include <QTimer>
#include <QStandardPaths>
#include <QSettings>
#include <QApplication>

ImageReply::ImageReply(const QUrl &url) :
    QNetworkReply(),
    offset(0)
{
    qDebug() << "ImageReply::ImageReply(" << url << ")";
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

ImageReply::~ImageReply()
{
    qDebug() << "ImageReply::~ImageReply()";
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
    setObjectName("NetworkManager");
    cache = new QNetworkDiskCache(this);

    qDebug() <<  QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QString cache_loc = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    cache->setCacheDirectory(cache_loc);
    cache->clear();

    this->setCache(cache);
}

NetworkManager::~NetworkManager()
{
    cache->clear();
}

void NetworkManager::setCacheSize(int MB)
{
    qDebug() << "NetworkManager::setCacheSize(" << MB << ")";
    cache->setMaximumCacheSize(MB * 1024 * 1024);

    emit cacheSize(MB);
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

void NetworkManager::save()
{
    qDebug() << "NetworkManager::save()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("cache.size", this->cache->maximumCacheSize() / 1024 / 1024);
}

void NetworkManager::load()
{
    qDebug() << "NetworkManager::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->setCacheSize(settings.value("cache.size", 0).toInt());
}


FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent)
{
    connect(&network_manager, SIGNAL(finished(QNetworkReply*)), SLOT(fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(imageUrl);
    network_manager.get(request);
}

FileDownloader::~FileDownloader()
{
    qDebug() << "FileDownloader::~FileDownloader()";
}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    downloaded_data = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded(downloaded_data);
}

QByteArray FileDownloader::downloadedData() const
{
    return downloaded_data;
}
