#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QString appPath = QCoreApplication::applicationDirPath();

    w.setWindowTitle("我的相册");
    w.resize(800,600);
    w.show();
    return a.exec();
}
