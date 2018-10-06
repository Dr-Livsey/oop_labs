#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QCloseEvent>
#include <QList>

#include <QBoxLayout>
class PlayerController;
class QListWidgetItem;
class CreatePlaylistPopup;
class MyVideoWidget;

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
    void AddPlaylist(QMediaPlaylist *, bool &added);

    void OpenVideo(QUrl &);

public slots:
    void UpdateDurationInfo(int duration);
    void UpdatePositionInfo(int position);

    void UpdateListWidget(const QList<QUrl> &NewPlaylist);
    void UpdateplListWidget(QMediaPlaylist *new_playlist);

    void PlaySelectedVideo(QListWidgetItem *video);
    void DeleteVideoFromPlaylist(int PlaylistIndex);
    void ResetPlaylist();
    void CreatePlaylist();

    void SetNewVolume(int);

    void currentVideoChanged(int);

    void closeEvent(QCloseEvent *event);
private:
    Ui::PlayerView *ui;
    MyVideoWidget *VideoWidget;
    QMediaPlayer *Player;

    QString intToTime(int itime);

    void showPlaylist(QMediaPlaylist *);
    QMediaPlaylist *GetPlaylistFromItem(QListWidgetItem*);
    void AddPlaylistToContainer(QMediaPlaylist *playlist, QString PlName);
    QMediaPlaylist *GetCurrentPlaylist();

    CreatePlaylistPopup *CreatePopUp;

    void setButtonStyle(QString, QString, QString, QString);
    void setBlueTintTheme();
    void setWhiteTheme();
};

void SetupController(PlayerController *Controller, PlayerView *View);

#endif // PLAYERVIEW_H
