#ifndef TIMESTAMPIMAGE_H
#define TIMESTAMPIMAGE_H

#include <QGraphicsItem>
#include <QFont>

class TimestampImage : public QGraphicsItem
{
public:
    TimestampImage(const QString& _timestamp,  const QFont& _font);
    void paint(QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;

private:
    QString mTitle;
    QFont mFont;
    int mFontWidth;
    int mFontHeight;
    QRectF mRect;
};

#endif // TIMESTAMPIMAGE_H
