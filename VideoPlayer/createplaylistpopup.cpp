#include "createplaylistpopup.h"
#include "ui_createplaylistpopup.h"
//#include <QDebug>

CreatePlaylistPopup::CreatePlaylistPopup(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::CreatePlaylistPopup)
{
    ui->setupUi(this);
    ui->Errorlabel->hide();
    ui->Errorlabel->setStyleSheet("QLabel { color : red; }");

    PlaylistName = "";

    setFixedSize(273, 105);
    setWindowTitle("Create Playlist");
    setWindowFlags(windowFlags() ^ Qt::WindowFullscreenButtonHint);
    setWindowFlags(windowFlags() ^ Qt::WindowMinimizeButtonHint);

    connect(ui->CancelButton, &QPushButton::clicked,
    [this]
    {
        close();
        emit PlaylistSetted();
    });
   // connect(ui->OkButton, SIGNAL(clicked()), this, SLOT(on_OkButton_clicked()));
}

CreatePlaylistPopup::~CreatePlaylistPopup()
{
    delete ui;
}

QString CreatePlaylistPopup::GetPlaylistName()
{
    QString RetVal = PlaylistName;
    ui->NamelineEdit->setText("");
    PlaylistName = "";

    return RetVal;
}

void CreatePlaylistPopup::on_OkButton_clicked()
{
    if (ui->NamelineEdit->text() == "")
    {
       ui->Errorlabel->show();
    }
    else
    {
        PlaylistName = ui->NamelineEdit->text();
        close();
        emit PlaylistSetted();
    }
}
