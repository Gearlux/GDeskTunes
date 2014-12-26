#include "downloader.h"

#include <QFileDialog>
#include <QUrl>
#include <QProcess>

Downloader::Downloader(QWidget *parent) : QObject(parent)
{

}

Downloader::~Downloader()
{

}

void Downloader::onUnsupportedContent(QNetworkReply * reply)
{
    qDebug() << "I guess we have to download " << reply->url();

    QString path = reply->url().path();
    QString filename = QFile(path).fileName();

    QString saved_filename = QFileDialog::getSaveFileName(dynamic_cast<QWidget*>(parent()), "Download and run file", filename);

    if (saved_filename != QString::null)
    {
        QByteArray data = reply->readAll();

        QFile file(saved_filename);
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();

        QProcess *process = new QProcess(this);
        process->start(saved_filename);
        connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));
    }
}
