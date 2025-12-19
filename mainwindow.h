#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void close();
    void init();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::vector<std::pair<QString, QString>> album;
};
#endif // MAINWINDOW_H
