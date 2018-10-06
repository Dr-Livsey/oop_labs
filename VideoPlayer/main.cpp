#include "playerview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlayerView View;

    View.show();

    return a.exec();
}
