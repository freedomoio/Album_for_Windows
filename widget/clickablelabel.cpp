#include "clickablelabel.h"

void ClickableLabel::mouseReleaseEvent(QMouseEvent* event){

    if (rect().contains(event->pos()) && event->button() == Qt::LeftButton) {
        // qDebug() << "缩略图被点击！";
        emit clicked(this->idx, this->photo);
    }
    QLabel::mouseReleaseEvent(event);
}
