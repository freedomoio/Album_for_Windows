#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QPixmap>
#include <QEvent>
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
#include <QtMinMax>
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
            // qDebug() << "缩略图被点击！";
            emit clicked();
        }
        QLabel::mouseReleaseEvent(event);
    }

signals:
    void clicked();
};

class PhotoWindowEventFilter : public QObject
{
    Q_OBJECT

public:
    PhotoWindowEventFilter(QPixmap bigPix,QLabel* bigLabel,QRect screenRect,QObject *parent = nullptr):
        QObject(parent),m_bigPix(bigPix),m_bigLabel(bigLabel),m_screenRect(screenRect) {}

protected:
    bool eventFilter(QObject *obj,QEvent *event)override{
        QWidget *win = qobject_cast<QWidget*>(obj);
        if(!win || !m_bigLabel){
            return QObject::eventFilter(obj,event);
        }

        //左键拖拽
        static bool isDragging =false;
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton && obj == win){
                m_dragStartPos = mouseEvent->globalPosition().toPoint() - win->pos();
                isDragging = true;
            }
            return true;
        }
        else if (event->type() == QEvent::MouseMove){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(isDragging){
                win->move(mouseEvent->globalPosition().toPoint() - m_dragStartPos);
            }
            return true;
        }
        else if(event->type() == QEvent::MouseButtonRelease){
            if(static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton){
                isDragging =false;
            }
            return true;
        }
        //缩放
        else if(event->type() == QEvent::Wheel){
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            int delta = wheelEvent->angleDelta().y() / 120;
            qreal scaleFactor = delta >0 ? 1.2 :0.8;

            QPoint centerPos = win->geometry().center();

            int newWidth = qBound(300,int(win->width() * scaleFactor),m_screenRect.width() - 100);
            int newHeight = qBound(200,int(win->height() * scaleFactor),m_screenRect.height() -100);

            int newX = centerPos.x() - newWidth / 2;
            int newY = centerPos.y() - newHeight /2;

            win->setGeometry(newX,newY,newWidth,newHeight);

            m_bigLabel->setGeometry(0,0,newWidth,newHeight);
            QPixmap scaleBig = m_bigPix.scaled(win->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            m_bigLabel->setPixmap(scaleBig);
            return true;
        }
        // ESC退出
        else if(event->type() == QEvent::KeyPress){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Escape){
                win->close();
                return true;
            }
        }

        return QObject::eventFilter(obj,event);

    }

private:
    QPixmap m_bigPix;
    QLabel* m_bigLabel;
    QRect m_screenRect;
    QPoint m_dragStartPos;
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

    QStringList g_photoPaths;

    std::vector<std::pair<QString, QString>> album;
};
#endif // MAINWINDOW_H
