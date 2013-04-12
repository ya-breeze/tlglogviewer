#include "timestampimage.h"

#include <QFontMetrics>
#include <QPainter>

TimestampImage::TimestampImage(const QString &_timestamp, const QFont &_font) :
    QGraphicsItem(), mTitle(_timestamp)
{
    QFontMetrics fm(mFont);
    mFontHeight = fm.height()/2;
    mFontWidth  = fm.width(mTitle);
    mRect = QRectF( QPointF(0,-mFontHeight*1.5), QPointF(mFontWidth, mFontHeight*1.5));
}

void TimestampImage::paint(QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    _painter->setPen( Qt::black );
    _painter->setFont(mFont);
    _painter->drawText( mRect, Qt::AlignLeft|Qt::AlignVCenter, mTitle );
}

QRectF TimestampImage::boundingRect() const
{
    return mRect;
}
