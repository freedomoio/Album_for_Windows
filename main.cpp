#include "window/mainwindow.h"

#include <QApplication>
#include <QImage>
#include <QFile>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QString appPath = QCoreApplication::applicationDirPath();

    w.setWindowTitle("我的相册");
    w.resize(1000,600);
    w.show();
    return a.exec();
}
