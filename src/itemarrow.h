#ifndef ITEMARROW_H
#define ITEMARROW_H

#include <QGraphicsItem>
#include <QString>
#include <QPointer>
#include <QFont>


#include "transaction.h"

class ItemArrow : public QGraphicsItem
{
public:
    explicit ItemArrow(const QPointer<Transaction>& _transaction, bool _leftToRight, int _width, const QColor &_color, const QFont& _font);
    void paint(QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *_event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    QPointer<Transaction> getTransaction() const;

signals:

public slots:

protected:
    int mWidth;
    QColor mColor;
    QFont mFont;
    int mFontHeight;
    bool mLeftToRight;
    QPointer<Transaction> ptrTransaction;
};

#endif // ITEMARROW_H
