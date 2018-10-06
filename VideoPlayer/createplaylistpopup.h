#ifndef CREATEPLAYLISTPOPUP_H
#define CREATEPLAYLISTPOPUP_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDebug>

namespace Ui {
class CreatePlaylistPopup;
}

class CreatePlaylistPopup : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreatePlaylistPopup(QMainWindow *parent = nullptr);
    ~CreatePlaylistPopup();

    QString GetPlaylistName();

signals:
    void PlaylistSetted();

private slots:
    void on_OkButton_clicked();
    void closeEvent(QCloseEvent *e)
    {
        if (e->spontaneous() == true) emit PlaylistSetted();
    }

private:
    Ui::CreatePlaylistPopup *ui;
    QString PlaylistName;
};

#endif // CREATEPLAYLISTPOPUP_H
