/*!
    主界面相关显示应用
*/
#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    w.setWindowTitle(QString::fromUtf8("远程界面管理"));
    w.show();
    return a.exec();
}
