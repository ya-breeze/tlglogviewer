#include "itemarrow.h"

#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QDebug>
#include <QMenu>
#include <QAction>

#include "callflow.h"

ItemArrow::ItemArrow(const QPointer<Transaction> &_transaction, bool _leftToRight, int _width, const QColor &_color,
                     const QFont& _font):
    QGraphicsItem(), mWidth(_width), mColor(_color),
    mFont(_font), mLeftToRight(_leftToRight), ptrTransaction(_transaction)
{
    QFontMetrics fm(mFont);
    mFontHeight = fm.height()/2;

    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
}

void ItemArrow::paint(QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPolygonF Triangle;
    if(mLeftToRight) {
        Triangle.append(QPointF(0,mFontHeight*1));
        Triangle.append(QPointF(mWidth-30,mFontHeight*1));
        Triangle.append(QPointF(mWidth-30,mFontHeight*1.5));
        Triangle.append(QPointF(mWidth,0));
        Triangle.append(QPointF(mWidth-30,-mFontHeight*1.5));
        Triangle.append(QPointF(mWidth-30,-mFontHeight*1));
        Triangle.append(QPointF(0,-mFontHeight*1));
    } else {
        Triangle.append(QPointF(mWidth, mFontHeight*1));
        Triangle.append(QPointF(30,mFontHeight*1));
        Triangle.append(QPointF(30,mFontHeight*1.5));
        Triangle.append(QPointF(0,0));
        Triangle.append(QPointF(30,-mFontHeight*1.5));
        Triangle.append(QPointF(30,-mFontHeight*1));
        Triangle.append(QPointF(mWidth,-mFontHeight*1));
    }
    _painter->setBrush(mColor);
    _painter->drawPolygon(Triangle);

    QString text = ptrTransaction->getArrowText();
    QFontMetrics fm(mFont);
    QRect rect(40, -mFontHeight, qMin(fm.width(text)+10, mWidth-80), mFontHeight*2);
    _painter->setBrush(Qt::white);
    _painter->setPen( Qt::black );
    _painter->drawRoundedRect(rect, 0, 0);

    if( !ptrTransaction->isOk() )
        _painter->setPen( Qt::red );
    _painter->setFont(mFont);
    QTextOption options(Qt::AlignHCenter|Qt::AlignVCenter);
    options.setWrapMode(QTextOption::WrapAnywhere);
    _painter->drawText( rect, text, options );

    if( isSelected() ) {
        _painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        QPen pen(Qt::white);
        pen.setWidth(5);
        _painter->setPen( pen );
        _painter->drawPolyline(Triangle);
    }
}

QRectF ItemArrow::boundingRect() const
{
    return QRectF( QPointF(0,-mFontHeight*1.5), QPointF(mWidth, mFontHeight*1.5));
}

void ItemArrow::contextMenuEvent(QGraphicsSceneContextMenuEvent *_event)
{
    qDebug() << "context menu";
    QMenu menu;
    QAction *filterAction = menu.addAction("Use session as filter - NOT implemented yet");
    QAction *getPairAction = menu.addAction("Move to pair transaction");
    QAction *selectSession = menu.addAction("Select whole session");
    QAction *selectedAction = menu.exec(_event->screenPos());
    if(selectedAction==getPairAction) {
        qDebug() << "Looking for a pair transaction";
        ptrTransaction->callflow()->selectPairTransaction(this);
    } else if(selectedAction==selectSession) {
        qDebug() << "Selecting whole session";
        ptrTransaction->callflow()->selectSession(ptrTransaction->getSessionId());
    } else if(selectedAction==filterAction) {
        qDebug() << "TODO Append session to filter";
    }
}

void ItemArrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

QPointer<Transaction> ItemArrow::getTransaction() const
{
    return ptrTransaction;
}
