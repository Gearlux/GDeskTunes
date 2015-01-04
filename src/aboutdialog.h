#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QNetworkReply>

#include "mediakey.h"
#include "mainwindow.h"

// Forward declarations
class VersionCheck;

namespace Ui {
class AboutDialog;
}

/**
 * @brief The AboutDialog class.
 *
 * The About Dialog implements a custom dialog which checks
 * for newer version on github.
 *
 * If you create a dialog and don't call show() before checking
 * for updates, the dialog will automically show() if updates are
 * available. If no updates are available or no connection with the
 * internet can be made, this dialog will automatically close.
 * (unless of course, you have called the show() function first)
 */
class AboutDialog : public MediaKeySource<QDialog, MainWindow>
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void checkForUpdates();

private slots:
    void donate();
    void onFinished(VersionCheck *version);

private:
    void setText(QString msg, QString link);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
