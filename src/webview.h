#ifndef WEBVIEW_H
#define WEBVIEW_H

#ifdef USE_WEBKIT
#include <QWebView>
#else
#include <QWebEngineView>
#endif

class WebView :
#ifdef USE_WEBKIT
        public QWebView
#else
        public QWebEngineView
#endif
{
    Q_OBJECT

public:
    WebView(QWidget* parent);
    ~WebView() {}

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:
    void setDraggable(bool drag);

private:
    bool draggable;
    bool move;
    int mouse_click_x_coordinate;
    int mouse_click_y_coordinate;
    QPoint offset;

};

#endif // WEBVIEW_H
