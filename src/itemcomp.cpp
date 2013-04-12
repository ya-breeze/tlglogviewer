#include "itemcomp.h"

#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QDrag>
#include <QApplication>
#include <QMimeData>
#include <QPixmap>
#include <QPainter>
#include <QBitmap>

#include "ICallFlow.h"

ItemComp::ItemComp(ICallFlow *_callflow, const QString &_text, const QFont &_font, int _height):
    QGraphicsObject(), pCallflow(_callflow), mText(_text), mFont(_font), mHeight(_height), mDragOver(false)
{
    QFontMetrics fm(mFont);
    mFontHeight = fm.height()/2;

    setFlags(QGraphicsItem::ItemIsMovable);
    setAcceptDrops(true);
    setToolTip(_text);
}

void ItemComp::paint(QPainter *_painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    _painter->drawLine(0, 0, 0, mHeight);
    _painter->setBrush(Qt::white);
    _painter->drawRect( QRect(-50, -mFontHeight*3, 100, mFontHeight*6) );
    _painter->drawText(QRect(-50, -mFontHeight*3, 100, mFontHeight*6), mText,
                       QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));
}

QRectF ItemComp::boundingRect() const
{
    return QRectF(-50, -mFontHeight*3, 100, mFontHeight*6+mHeight);
}

void ItemComp::contextMenuEvent(QGraphicsSceneContextMenuEvent *_event)
{
    qDebug() << "context menu";
    QMenu menu;
    QAction *removeAction = menu.addAction("Hide");
    QAction *selectedAction = menu.exec(_event->screenPos());
    if( selectedAction==removeAction ) {
        pCallflow->hideComponent(mText);
    }
}

void ItemComp::setHeight(int _value)
{
    mHeight = _value;
    update(boundingRect());
}

QString ItemComp::title() const
{
    return mText;
}

//void ItemComp::droppedComp(const QString &_compDroppedOn, const QString &_compDropped)
//{
//    qDebug() << "!!!!!!!!!!!!!!!!!!!!";
//}

void ItemComp::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::ClosedHandCursor);
}

void ItemComp::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mouseMove";
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
            .length() < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    mime->setData(MIME, mText.toUtf8());
    drag->setMimeData(mime);

    /*
        mime->setColorData(color);
        mime->setText(QString("#%1%2%3")
                      .arg(color.red(), 2, 16, QLatin1Char('0'))
                      .arg(color.green(), 2, 16, QLatin1Char('0'))
                      .arg(color.blue(), 2, 16, QLatin1Char('0')));

*/
    QPixmap pixmap( 110, mFontHeight*9+50 );
    pixmap.fill(Qt::yellow);

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawLine(50, 0, 50, 100);
    painter.setBrush(Qt::white);
    painter.drawRect( QRect(1, 1, 100, mFontHeight*6) );
    painter.drawText(QRect(1, 1, 100, mFontHeight*6), mText,
                     QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(0, 0));
    drag->exec();
    setCursor(Qt::OpenHandCursor);
}

void ItemComp::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::OpenHandCursor);
}

void ItemComp::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(MIME)
            && QString(event->mimeData()->data(MIME))!=mText ) {
        event->setAccepted(true);
        mDragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void ItemComp::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    mDragOver = false;
    update();
}

void ItemComp::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    mDragOver = false;
    if (event->mimeData()->hasFormat(MIME)) {
        QString dropped = event->mimeData()->data(MIME);
        qDebug() << "Dropped" << dropped << "onto" << mText;
        emit droppedComp(mText, dropped);
    }
    update();
}
