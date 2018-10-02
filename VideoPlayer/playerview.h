#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QList>

#include <QBoxLayout>
class PlayerController;
class QListWidgetItem;

/*TODO:
  1. Load new playlist.
  2. Save current playlist.
  3. Video information.
  4. Rewind.
  5. Theme settings. */

namespace Ui {
class PlayerView;
}

class PlayerView : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerView
    (
        QWidget *parent = nullptr
    );

    friend void SetupController(PlayerController *Controller, PlayerView *View);

     ~PlayerView();
signals:
    void SavePlaylist(QMediaPlaylist *);
    void AddPlaylist(QMediaPlaylist *);

public slots:
    void UpdateDurationInfo(int duration);
    void UpdatePositionInfo(int position);

    void UpdateListWidget(const QList<QUrl> &NewPlaylist);
    void UpdateplListWidget(QMediaPlaylist *new_playlist);
    void PlaySelectedVideo(QListWidgetItem *video);
    void DeleteVideoFromPlaylist(int PlaylistIndex);
    void ResetPlaylist();

    void SetNewVolume(int);

    void currentVideoChanged(int);

private:
    Ui::PlayerView *ui;
    QVideoWidget *VideoWidget;
    QMediaPlayer *Player;

    QMediaPlaylist *Playlist;
    QList<QMediaPlaylist*> PlList;

    QString intToTime(int itime);
};

void SetupController(PlayerController *Controller, PlayerView *View);

#endif // PLAYERVIEW_H
