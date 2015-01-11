#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpSocket>

#include "bonjourrecord.h"

class QDialogButtonBox;
class QPushButton;
class QLabel;
class QTreeWidget;
class Client;


class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

    void closeEvent(QCloseEvent *event);

private slots:
    void updateRecords(const QList<BonjourRecord> &list);
    void requestNewFortune();
    void displayError(QAbstractSocket::SocketError socketError, const QString &error);
    void enableConnectButton();

    void setFortune(const QString &nextFortune);

    void onConnectClicked();
    void onDisconnectClicked();
    void onConnected();
    void onDisconnected();

private:
    QLabel *statusLabel;
    QPushButton *getFortuneButton;
    QPushButton *quitButton;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QDialogButtonBox *buttonBox;

    QString currentFortune;
    QTreeWidget *treeWidget;

    Client *client;
};

#endif // DIALOG_H
