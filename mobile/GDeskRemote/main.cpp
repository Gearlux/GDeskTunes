#include <QtQml>

#include <QApplication>
#include <QQmlApplicationEngine>

#include "remoteclient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<RemoteClient>("org.gearlux.gdeskremote", 1, 0, "RemoteClient");
    QQmlApplicationEngine engine(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
