#ifndef ITEMCOMP_H
#define ITEMCOMP_H

#include <QGraphicsItem>
#include <QFont>

#define MIME "application/tlgLog"

class ICallFlow;

class ItemComp : public QGraphicsObject
{
    Q_OBJECT
public:
    ItemComp(ICallFlow *_callflow, const QString& _text, const QFont& _font, int _height);
    void paint(QPainter *_painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *_event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

    void setHeight(int _value);
    QString title() const;

signals:
    void droppedComp(const QString& _compDroppedOn, const QString& _compDropped);

public slots:

protected:
    ICallFlow *pCallflow;
    QString mText;
    QFont mFont;
    int mFontHeight;
    int mHeight;
    bool mDragOver;
};

#endif // ITEMCOMP_H
