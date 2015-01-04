#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "versioncheck.h"

#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) :
    MediaKeySource<QDialog, MainWindow>(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
#ifdef Q_OS_LINUX
    setWindowFlags(Qt::Tool);
#else
    setWindowFlags(Qt::Dialog
                   | Qt::WindowCloseButtonHint
                   | Qt::CustomizeWindowHint
                   );
#endif

    ui->title->setText(QApplication::applicationDisplayName());
    ui->version->setText("Version " + QApplication::applicationVersion());

    ui->latest_version->hide();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::checkForUpdates()
{
    ui->latest_version->setVisible(true);
    ui->latest_version->setText("Checking for updates ...");
    ui->spinner->setLineLength(8);
    ui->spinner->start();

    VersionCheck *version = new VersionCheck();
    version->setUrl(QUrl("https://api.github.com/repos/Gearlux/GDeskTunes/releases"));

    QObject::connect(version, SIGNAL(finished(VersionCheck*)), this, SLOT(onFinished(VersionCheck*)));

    version->check();
}

void AboutDialog::onFinished(VersionCheck *version)
{
    ui->spinner->stop();
    ui->spinner->hide();

    setText(version->getMessage(), version->getAction());

    if (!isVisible())
    {
        if (version->hasUpdates())
        {
            show();
        }
        else
            close();
    }

}

void AboutDialog::setText(QString msg, QString msg2)
{
    QString html = "<p style='text-align: center'>" + msg + "<br><a href='http://gearlux.github.io/GDeskTunes/'>" + msg2 + "</a></p>";
    ui->latest_version->setVisible(true);
    ui->latest_version->setText(html);
}

void AboutDialog::donate()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=9DG9CCT9C7GQN&lc=BE&item_name=GearLux&item_number=GDeskTunes&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted"));
}
