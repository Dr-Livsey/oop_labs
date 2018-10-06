#ifndef MYVIDEOWIDGET_H
#define MYVIDEOWIDGET_H

#include <QVideoWidget>

class QMouseEvent;
class QKeyEvent;
class QMediaPlayer;

class MyVideoWidget : public QVideoWidget
{
public:
    explicit MyVideoWidget(QMediaPlayer *Player, QWidget *parent = nullptr);

    void mouseDoubleClickEvent(QMouseEvent *me);
    void keyPressEvent(QKeyEvent *key_event);
private:
    QMediaPlayer *MediaPlayer;
};

#endif // MYVIDEOWIDGET_H
