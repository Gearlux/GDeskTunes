#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkDiskCache>

class ImageReply: public QNetworkReply
{
    Q_OBJECT
public:
    ImageReply(const QUrl &url);
    virtual ~ImageReply();
    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;

public slots:
    void deferEmitSignals();

protected:
    qint64 readData(char *data, qint64 maxSize);
    QByteArray read(qint64 maxSize);
    QByteArray readAll();

private:
    qint64 offset;
    QByteArray content;
};

class NetworkManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = 0);
    virtual ~NetworkManager();

signals:
    void cacheSize(int MB);

public slots:
    QNetworkReply* createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0);
    void setCacheSize(int MB);
    void load();
    void save();

private:
    QNetworkDiskCache *cache;
};

class FileDownloader: public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl imageURL, QObject *parent);
    virtual ~FileDownloader();

    QByteArray downloadedData() const;

signals:
    void downloaded(QByteArray &data);

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QNetworkAccessManager network_manager;
    QByteArray downloaded_data;
};

#endif // NETWORKACCESSMANAGER_H
