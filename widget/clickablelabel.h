#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel :public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent = nullptr, const std::vector<QString>* arr = nullptr):QLabel(parent), photo(arr){
        setMouseTracking(true);
        setAttribute(Qt::WA_AcceptTouchEvents,true);
    }
    inline int getIdx(){ return this->idx; }
    inline void setIdx(int idx){ this->idx = idx; }

protected:
    int idx;
    const std::vector<QString>* photo;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void clicked(int idx, const std::vector<QString>*);
};

#endif // CLICKABLELABEL_H
