#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include <QWidget>
#include <QList>

class QMediaPlaylist;
class QListWidgetItem;

class PlayerModel : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerModel(QWidget *parent = nullptr);

    bool isPlaylist(const QUrl &url);

signals:
    void VideoAddedToPlaylist(const QList<QUrl> &PList);

public slots:
    /*Playlist*/
    void AddVideoToPlaylist();
    /*Open video. Dont append to playlist.*/
    void OpenVideo(QUrl &);
    /*Save Playlist*/
    void SavePlaylist(QMediaPlaylist *);
    /*Add Playlist*/
    void AddPlaylist(QMediaPlaylist *, bool &added);
};

#endif // PLAYERMODEL_H
