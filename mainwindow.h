#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QSplitter>
#include <QList>
#include <QStringList>
#include <QStatusBar>
#include <QToolBar>
#include <QListWidget>
#include <QResizeEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QJsonObject>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include<QFileDialog>
#include <QFileInfo>
#include <vector>
#include <QRect>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ClickableLabel :public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent = nullptr):QLabel(parent){
        setMouseTracking(true);
        setAttribute(Qt::WA_AcceptTouchEvents,true);
    }

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {

        if (rect().contains(event->pos()) && event->button() == Qt::LeftButton) {
            qDebug() << "缩略图被点击！";
            emit clicked();
        }
        QLabel::mouseReleaseEvent(event);
    }

signals:
    void clicked();
};

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
