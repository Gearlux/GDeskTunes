#include "mediakeydialog.h"
#include "ui_mediakeydialog.h"

MediaKeyDialog::MediaKeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MediaKeyDialog)
{
    ui->setupUi(this);
}

MediaKeyDialog::~MediaKeyDialog()
{
    delete ui;
}
