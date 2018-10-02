#include "playerview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlayerView *View = new PlayerView;

    View->show();

    return a.exec();
}
