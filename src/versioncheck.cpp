#include "versioncheck.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVector>


// Copied shamelessly from QT 5.4 toolkit
class VersionNumber
{
public:
    VersionNumber(const QVector<int> &seg) : m_segments(seg) {}

public:
    QString toString() const
    {
        QString version;
        version.reserve(qMax(m_segments.size() * 2 - 1, 0));
        bool first = true;
        for (QVector<int>::const_iterator it = m_segments.begin(), end = m_segments.end(); it != end; ++it) {
            if (!first)
                version += QLatin1Char('.');
            version += QString::number(*it);
            first = false;
        }
        return version;
    }
private:
    QVector<int> m_segments;

public:
    static int compare(const VersionNumber &v1, const VersionNumber &v2)
    {
        QVector<int>::const_iterator i1 = v1.m_segments.constBegin();
        const QVector<int>::const_iterator e1 = v1.m_segments.constEnd();
        QVector<int>::const_iterator i2 = v2.m_segments.constBegin();
        const QVector<int>::const_iterator e2 = v2.m_segments.constEnd();

        while (i1 != e1 && i2 != e2) {
            if (*i1 != *i2)
                return (*i1 - *i2);
            ++i1;
            ++i2;
        }

        // ran out of segments in v1 and/or v2 and need to check the first trailing
        // segment to finish the compare
        if (i1 != e1) {
            // v1 is longer
            if (*i1 != 0)
                return *i1;
            else
                return 1;
        } else if (i2 != e2) {
            // v2 is longer
            if (*i2 != 0)
                return -*i2;
            else
                return -1;
        }

        // the two version numbers are the same
        return 0;
    }

    static VersionNumber fromString(const QString &string)
    {
        QVector<int> seg;

        const QByteArray cString(string.toLatin1());

        const char *start = cString.constData();
        const char *end = start;
        const char *lastGoodEnd = start;
        const char *endOfString = cString.constData() + cString.size();

        do {
            const qulonglong value = strtoull(start, (char**)&end, 10);
            seg.append(int(value));
            start = end + 1;
            lastGoodEnd = end;
        } while (start < endOfString && (end < endOfString && *end == '.'));

        return VersionNumber(qMove(seg));
    }
};


VersionCheck::VersionCheck(QObject *parent) :
    QObject(parent),
    has_updates(false),
    message(QString::null),
    action(QString::null)
{

}

VersionCheck::~VersionCheck()
{

}

void VersionCheck::setUrl(const QUrl &url)
{
    this->url = url;
}

void VersionCheck::check()
{
    QNetworkAccessManager* network_manager = new QNetworkAccessManager(this);
    QObject::connect(network_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));
    network_manager->get(QNetworkRequest(this->url));
}

void VersionCheck::setText(const QString &msg, const QString &action)
{
    this->message = msg;
    this->action = action;
}

void VersionCheck::onFinished(QNetworkReply *reply)
{
    has_updates = false;
    if (reply->error() == QNetworkReply::NoError)
    {
        QString strReply = (QString)reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

        if (jsonResponse.isArray())
        {
            QJsonArray jsonArray = jsonResponse.array();
            if (jsonArray.count() > 0)
            {
                QJsonValue first = jsonArray.at(0);
                QString tag_name = first.toObject().take("tag_name").toString();

                VersionNumber online_version = VersionNumber::fromString(tag_name.right(tag_name.length()-1));
                VersionNumber this_version = VersionNumber::fromString(QApplication::applicationVersion());

                qDebug() << "Online: " << online_version.toString() << " Running: " << this_version.toString();

                if (VersionNumber::compare(online_version, this_version) <= 0)
                {
                    setText("You are running the latest version:", tag_name);
                }
                else
                {
                    setText("New version available: " + tag_name, "Download it now");
                    has_updates = true;
                }
           }
            else
            {
                setText("Unable to find any release from GitHub.", "Check the homepage.");
            }
        }
        else
        {
            setText("Unable to process data from GitHub.", "Check the homepage");
        }
    }
    else
    {
        setText("Unable to obtain response from GitHub.", "Check the homepage");
    }

    emit finished(this);
}
