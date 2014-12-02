#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "mainwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog
                   | Qt::WindowCloseButtonHint
#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
                   | Qt::CustomizeWindowHint
#endif
                   );
}

AboutDialog::~AboutDialog()
{
    delete ui;
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
