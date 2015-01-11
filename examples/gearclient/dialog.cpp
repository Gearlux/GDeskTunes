#include "dialog.h"

#include <QLabel>
#include <QTreeWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QGridLayout>

#include "client.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << tr("Available Fortune Servers"));

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    connectButton->setEnabled(false);

    disconnectButton = new QPushButton(tr("Disconnect"));
    disconnectButton->setDefault(true);
    disconnectButton->setEnabled(false);

    getFortuneButton = new QPushButton(tr("Get Fortune"));
    getFortuneButton->setDefault(true);
    getFortuneButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(disconnectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(treeWidget, 0, 0, 2, 2);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Fortune Client"));
    treeWidget->setFocus();

    connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
    connect(getFortuneButton, SIGNAL(clicked()), this, SLOT(requestNewFortune()));
    connect(disconnectButton, SIGNAL(clicked()),  this, SLOT(onDisconnectClicked()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    client = new Client(this);

    connect(client, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SLOT(updateRecords(const QList<BonjourRecord> &)));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError,const QString &)),
            this, SLOT(displayError(QAbstractSocket::SocketError, const QString &)));

    connect(client, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

}

Dialog::~Dialog()
{

}

void Dialog::onConnected()
{
    getFortuneButton->setEnabled(true);
    disconnectButton->setEnabled(true);
}

void Dialog::onDisconnected()
{
    getFortuneButton->setEnabled(false);
    disconnectButton->setEnabled(false);
}
void Dialog::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

void Dialog::requestNewFortune()
{
    getFortuneButton->setEnabled(false);

    qDebug() << "Gui::requestNewFortune()";
    client->invokeMethod("requestNewFortune");
}


void Dialog::displayError(QAbstractSocket::SocketError socketError, const QString &error)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(error));
    }

    getFortuneButton->setEnabled(true);
}

void Dialog::enableConnectButton()
{
    connectButton->setEnabled(treeWidget->invisibleRootItem()->childCount() != 0);
}

void Dialog::updateRecords(const QList<BonjourRecord> &list)
{
    treeWidget->clear();
    foreach (BonjourRecord record, list) {
        QVariant variant;
        variant.setValue(record);
        QTreeWidgetItem *processItem = new QTreeWidgetItem(treeWidget,
                                                           QStringList() << record.serviceName);
        processItem->setData(0, Qt::UserRole, variant);
    }

    if (treeWidget->invisibleRootItem()->childCount() > 0) {
        treeWidget->invisibleRootItem()->child(0)->setSelected(true);
    }
    enableConnectButton();
}

void Dialog::setFortune(const QString &nextFortune)
{
    qDebug() << "Gui::setFortune(" << nextFortune << ")";
    currentFortune = nextFortune;
    statusLabel->setText(currentFortune);
    getFortuneButton->setEnabled(true);
}

void Dialog::onConnectClicked()
{
    if (client->isConnected())
        client->disconnect();

    QList<QTreeWidgetItem *> selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty())
        return;

    QTreeWidgetItem *item = selectedItems.at(0);
    QVariant variant = item->data(0, Qt::UserRole);

    client->connectToServer(variant.value<BonjourRecord>());
}

void Dialog::onDisconnectClicked()
{
    client->close();
}

