#ifndef VERSIONCHECK_H
#define VERSIONCHECK_H

#include <QObject>
#include <QUrl>

// Forward declarations
class QNetworkReply;

/**
 * @brief The VersionCheck class
 *
 * Utility class to check the version of the published releases
 * on GitHub.
 */
class VersionCheck : public QObject
{
    Q_OBJECT
public:
    explicit VersionCheck(QObject *parent = 0);
    ~VersionCheck();

signals:
    void finished(VersionCheck *version);

public slots:

public:
    void setUrl(const QUrl &url);

    void check();

    bool hasUpdates() { return has_updates; }
    QString getMessage() { return this->message; }
    QString getAction() { return this->action; }

private slots:
    void onFinished(QNetworkReply *reply);

private:
    void setText(const QString &msg, const QString &action);

private:
    QUrl url;
    bool has_updates;
    QString message;
    QString action;
};

#endif // VERSIONCHECK_H
