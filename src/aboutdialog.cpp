#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "mainwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    qDebug() << "AboutDialog::AboutDialog(" << parent << ")";
    ui->setupUi(this);
#ifdef Q_OS_LINUX
    qDebug() << "Linux";
    setWindowFlags(Qt::Tool);
#else
    setWindowFlags(Qt::Dialog
                   | Qt::WindowCloseButtonHint
#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
                   | Qt::CustomizeWindowHint
#endif
                   );
#endif

    ui->version->setText("Version " + QApplication::applicationVersion());

    ui->latest_version->setText("Checking for updates ...");
    ui->spinner->setLineLength(8);
    ui->spinner->start();

    QNetworkAccessManager* network_manager = new QNetworkAccessManager(this);
    QObject::connect(network_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkVersion(QNetworkReply*)));

    QUrl url = QUrl("https://api.github.com/repos/Gearlux/GDeskTunes/releases");
    network_manager->get(QNetworkRequest(url));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setText(QString msg, QString msg2)
{
    QString html = "<p style='text-align: center'>" + msg + "<br><a href='http://gearlux.github.io/GDeskTunes/'>" + msg2 + "</a></p>";
    ui->latest_version->setText(html);
}

void AboutDialog::checkVersion(QNetworkReply *reply)
{
   bool has_updates = false;
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

                if (tag_name.right(tag_name.length()-1) == QApplication::applicationVersion())
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

    ui->spinner->stop();
    ui->spinner->hide();

    if (!isVisible())
    {
        if (has_updates)
        {
            show();
        }
        else
            close();
    }
}

void AboutDialog::donate()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=9DG9CCT9C7GQN&lc=BE&item_name=GearLux&item_number=GDeskTunes&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted"));
}

void AboutDialog::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaNext:
    case Qt::Key_MediaPrevious:
    {
        MainWindow *w = qobject_cast<MainWindow*>(parent());
        if (w != 0)
        {
            qDebug() << "About dialog captures media key";
            w->keyPressEvent(event);
            break;
        }
    }
    default:
        QDialog::keyPressEvent(event);
        break;
    }
}
