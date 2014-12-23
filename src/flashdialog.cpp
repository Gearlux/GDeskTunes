#include "flashdialog.h"
#include "ui_flashdialog.h"

#include <QWebPage>
#include <QWebFrame>

FlashDialog::FlashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FlashDialog)
{
    ui->setupUi(this);

    ui->webView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);

    ui->webView->settings()->setUserStyleSheetUrl(QUrl("qrc:/resources/flash.css"));
}

FlashDialog::~FlashDialog()
{
    delete ui;
}
