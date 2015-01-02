#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QNetworkReply>

#include "keys.h"
#include "mainwindow.h"

namespace Ui {
class AboutDialog;
}

/**
 * @brief The AboutDialog class.
 *
 * The About Dialog implements a Dialog which can check for newer
 * versions on github.
 */
class AboutDialog : public KeyForwarder<QDialog, MainWindow>
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void donate();

    void checkVersion(QNetworkReply *reply);

private:
    void setText(QString msg, QString link);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
