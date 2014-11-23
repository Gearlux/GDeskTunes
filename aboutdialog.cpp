#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDesktopServices>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::donate()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=9DG9CCT9C7GQN&lc=BE&item_name=GearLux&item_number=GDeskTunes&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted"));
}
