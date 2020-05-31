#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowTitle(QString::fromLocal8Bit("远程设备管理"));
    w.show();
    return a.exec();
}
