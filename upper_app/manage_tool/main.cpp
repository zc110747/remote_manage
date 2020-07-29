/*!
    主界面相关显示应用
*/
#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowTitle(QString::fromUtf8("远程管理方案"));
    w.show();
    return a.exec();
}
