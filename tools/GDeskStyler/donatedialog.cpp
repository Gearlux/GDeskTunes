#include "donatedialog.h"
#include "ui_donatedialog.h"

DonateDialog::DonateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DonateDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
}

DonateDialog::~DonateDialog()
{
    delete ui;
}
