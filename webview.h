#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>
#include <QWebView>

class MainWindow;

class WebView : public QWebView
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:

private:
    bool move;
    int mouse_click_x_coordinate;
    int mouse_click_y_coordinate;

    MainWindow *main_window;
};

#endif // WEBVIEW_H
