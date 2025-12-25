#include "photowindoweventfilter.h"

bool PhotoWindowEventFilter::eventFilter(QObject *obj,QEvent *event){
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
    }//鼠标移动
    else if (event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(isDragging){//当处在拖动时
            win->move(mouseEvent->globalPosition().toPoint() - m_dragStartPos);
        }
        return true;
    }//鼠标放开
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
