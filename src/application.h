#ifndef APPLICATION_H
#define APPLICATION_H

#include "thirdparty/qt-solutions/qtsingleapplication/src/QtSingleApplication"

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv, bool GUIenabled = true);

    bool event(QEvent *);

signals:

public slots:

};

#endif // APPLICATION_H
