#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>

class WebView : public QWebView
{
    Q_OBJECT

public:
    WebView(QWidget* parent);
    ~WebView() {};

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
