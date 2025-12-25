#ifndef PHOTOWINDOWEVENTFILTER_H
#define PHOTOWINDOWEVENTFILTER_H

#include <QObject>
#include <QPixmap>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

class PhotoWindowEventFilter : public QObject
{
    Q_OBJECT

public:
    PhotoWindowEventFilter(QPixmap bigPix,QLabel* bigLabel,QRect screenRect,QObject *parent = nullptr):
        QObject(parent),m_bigPix(bigPix),m_bigLabel(bigLabel),m_screenRect(screenRect) {}
    PhotoWindowEventFilter(const PhotoWindowEventFilter&)=delete;
    PhotoWindowEventFilter(PhotoWindowEventFilter&&)=delete;

protected:
    bool eventFilter(QObject*, QEvent*)override;

private:
    QPixmap m_bigPix;
    QLabel* m_bigLabel;
    QRect m_screenRect;
    QPoint m_dragStartPos;
};

struct AlbumInfo
{
    QString albumName;
    QStringList photoPaths;

    AlbumInfo() = default;
    AlbumInfo(const QString& name,const QStringList& photos = {})
        : albumName(name),photoPaths(photos){}

};

#endif // PHOTOWINDOWEVENTFILTER_H
