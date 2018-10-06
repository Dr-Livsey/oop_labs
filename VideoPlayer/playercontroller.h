#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

class QPushButton;
class QSlider;
class PlayerView;
class QListWidget;
class QListWidgetItem;

#include <QWidget>
#include <QList>

class PlayerController : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerController(PlayerView *View = nullptr);

    friend void SetupController(PlayerController *Controller, PlayerView *View);

signals:
   void updDuration(int);
   void updPosition(int);

   void plUpdated(const QList<QUrl> &NewPlaylist);
   void deleteFromPlaylist(int PlaylistIndex);
   void ResetPlaylist();

   void SetNewVolumeValue(int);

public slots:
    void ChangeDuration(qint64);
    void ChangePlaybackPos(qint64);

    void PlaylistUpdated(const QList<QUrl> &NewPlaylist);
    void DeleteButtonPressed();
    void ResetListWidget();

    void VolumeValueChanged(int);

private:
    QPushButton *MuteButton;
    QPushButton *SaveButton;

    QPushButton *NextButton;
    QPushButton *PrevButton;

    QPushButton *PlayButton;
    QPushButton *StopButton;
    QPushButton *PauseButton;

    QPushButton *OpenButton;
    QPushButton *AddVideoButton;
    QPushButton *DeleteButton;
    QPushButton *ResetButton;

    QSlider *VolumeSlider;
    QSlider *DurationSlider;

    QListWidget *PlaylistWidget;
};

#endif // PLAYERCONTROLLER_H
