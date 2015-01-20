#include <QtQml>

#include <QApplication>
#include "engine.h"

#include "remoteclient.h"
#include "imageprovider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<RemoteClient>("org.gearlux.gdeskremote", 1, 0, "RemoteClient");

    Engine engine(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.addImageProvider("gdeskremote", new ImageProvider());

    return app.exec();
}
