#include "callflow.h"

#include <stdlib.h>

#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneEvent>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QApplication>

Callflow::Callflow()
    : QGraphicsScene(NULL), mNeedDrawTimestamps(true), mExpressionParser("")
{
    mFont.setPointSize(12);
    mSessionColors << "#c0c0c0" << "#00ff00" << "#ffff00" << "#0000ff" << "#00ffff"
                   << "#ff0000" << "#ff00ff" << "#800000" << "#808000" << "#008000"
                   << "#008080" << "#800080" << "#FFC0CB";

    connect(this, SIGNAL(selectionChanged()), SLOT(itemSelected()));
}

int Callflow::sizeTransactions() const
{
    return mTransactions.size();
}

QPointer<Transaction> Callflow::getTransaction(int _index) const
{
    return mTransactions.at(_index);
}

void Callflow::addTransaction(const QPointer<Transaction> &_value)
{
    if( !mComponents.contains(_value->getSource().getComp()) )
        addComponent(_value->getSource().getComp());
    if( !mComponents.contains(_value->getDest().getComp()) )
        addComponent(_value->getDest().getComp());
    mTransactions << _value;
}

void Callflow::addComponent(const QString& _name) {
    mComponents << _name;
}

bool Callflow::isCtrlPressed() const {
    QApplication *app = dynamic_cast<QApplication*>(QApplication::instance());
    return app->keyboardModifiers()&&Qt::ControlModifier;
}

int Callflow::sizeComponents() const
{
    return mComponents.size();
}

//QString Callflow::getComponent(int _index) const
//{
//    return mComponents.at(_index);
//}

void Callflow::clear()
{
    qDebug() << "Clearing";
    emit transactionSelected(NULL);

    foreach (QGraphicsItem *item, mItemArrows) {
        removeItem(item);
    }
    foreach (QGraphicsItem *item, mItemComps) {
        removeItem(item);
    }
    foreach (QGraphicsItem *item, mTimestamps) {
        removeItem(item);
    }
    qDeleteAll(mItemArrows);
    qDeleteAll(mItemComps);
    qDeleteAll(mTimestamps);

    mSessions.clear();
    mComponents.clear();
    mTransactions.clear();
    mItemArrows.clear();
    mItemComps.clear();
    mTimestamps.clear();

    QGraphicsScene::clear();
}

void Callflow::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    qDebug() << "mouse at " << event->scenePos();
    if( isCtrlPressed() ) {
        qreal s = event->delta()/1000.0;
        views().at(0)->scale(1+s, 1+s);
        views().at(0)->centerOn(event->scenePos());
    } else
        QGraphicsScene::wheelEvent(event);
}

//void Callflow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//    if( event->button()==Qt::LeftButton )
//        qDebug() << "Scene Clicked Left";
//    else if( event->button()==Qt::RightButton )
//        qDebug() << "Scene Clicked Right";
//}

void Callflow::contextMenuEvent(QGraphicsSceneContextMenuEvent *_event)
{
    QGraphicsScene::contextMenuEvent(_event);
    if( !_event->isAccepted() ) {
        qDebug() << "context menu";
        QMenu menu;
        QAction *removeAction  = menu.addAction("Reset filter");
        QAction *reverseAction = menu.addAction("Reverse");
        QAction *selectedAction = menu.exec(_event->screenPos());
        if( selectedAction==reverseAction ) {
            reverse();
            rebuild();
        } else if( selectedAction==removeAction ) {
            resetHidedComponents();
        }
    }
}

void Callflow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        QList<QGraphicsItem *> items = selectedItems();
        if( items.isEmpty() ) {
            qDebug() << "nothing is selected";
            return;
        }
        ItemArrow *arrow = dynamic_cast<ItemArrow*>(items.at(0));
        if( !arrow ) {
            qDebug() << "not arrow selected";
            return;
        }
        int index = mItemArrows.indexOf(arrow);
        if( index==-1 ) {
            qErrnoWarning("Internal error: arrow not in mItemArrows");
            return;
        }
        if(event->key() == Qt::Key_Up){
            if( index>0 ) {
                clearSelection();
                ItemArrow *arrowNew = mItemArrows.at(index-1);
                arrowNew->setSelected(true);
            }
        } else  if(event->key() == Qt::Key_Down){
            if( index<mItemArrows.size()-1 ) {
                clearSelection();
                ItemArrow *arrowNew = mItemArrows.at(index+1);
                arrowNew->setSelected(true);
            }
        }
    }
}

void Callflow::setFastFind(const QString &_filter) {
    mFastFilter = Filter(_filter);
}

const Filter &Callflow::getFastFind() const {
    return mFastFilter;
}

void Callflow::selectPairTransaction(const ItemArrow *_arrow)
{
    qDebug() << "Trying to find a pair transaction for"
             << _arrow->getTransaction()->getSource().toString() << "=>" << _arrow->getTransaction()->getDest().toString();
    for(int i=0; i<mItemArrows.size(); ++i) {
        ItemArrow *arrow = mItemArrows.at(i);
        if( arrow!=_arrow && arrow->getTransaction()->isPair(*_arrow->getTransaction().data()) ) {
            qDebug() << "finded a pair, index - " << i;
            clearSelection();
            arrow->setSelected(true);
            emit needCenter(arrow);
            break;
        }
    }

}

void Callflow::selectSession(const QString &_sessionId)
{
    qDebug() << "selecting session" << _sessionId;
    std::for_each(mItemArrows.begin(), mItemArrows.end(), [&_sessionId](ItemArrow *_arrow){
        if( _arrow->getTransaction()->getSessionId()==_sessionId)
            _arrow->setSelected(true);
    });
}

bool Callflow::needDrawTimestamp() const
{
    return mNeedDrawTimestamps;
}

void Callflow::setNeedDrawTimestamp(bool _value)
{
    qDebug() << "setNeedDrawTimestamp" << _value;
    mNeedDrawTimestamps = _value;

    std::for_each(mTimestamps.begin(), mTimestamps.end(), [_value](TimestampImage *_ts){_ts->setVisible(_value);} );
}

void Callflow::itemSelected()
{
    qDebug() << "selection changed";
    QList<QGraphicsItem *> items = selectedItems();
    if(items.isEmpty()) {
        qDebug() << "selection is cleared";
        emit transactionSelected(NULL);
        return;
    }

    ItemArrow *arrow = dynamic_cast<ItemArrow*>(items[0]);
    if( arrow ) {
        qDebug() << "arrow selected";
        emit transactionSelected(arrow);
    }
}

void Callflow::droppedComp(const QString &_compDroppedOn, const QString &_compDropped)
{
    if( _compDroppedOn==_compDropped )
        return;
    mCompDropped   = _compDropped;
    mCompDroppedOn = _compDroppedOn;
    // little hack - it's impossible to update here, because
    // active ItemComp will be deleted and we've got SIGSEGV,
    // so we just schedule an update() and it will be performed
    // as soon, as possible - in the first event processing in QApplication
    QTimer::singleShot(0, this, SLOT(delayedDroppedComp()));
}

void Callflow::delayedDroppedComp()
{
    // Trying swap instead of insert
    if( isCtrlPressed() ) {
        mComponents.removeAll(mCompDropped);
        int idxDroppedOn = mComponents.indexOf(mCompDroppedOn);
        mComponents.insert(idxDroppedOn, mCompDropped);
    } else {
        int idxDropped   = mComponents.indexOf(mCompDropped);
        int idxDroppedOn = mComponents.indexOf(mCompDroppedOn);
        mComponents.swap(idxDropped, idxDroppedOn);
    }

    rebuild();
}

void Callflow::find(Qt::CaseSensitivity _case, FindDirection _direction)
{
    qDebug() << "Searching with direction " << _direction;

    if( _direction!=REVERSE_DIRECTION ) {
        int begin = 0;
        if( _direction==FORWARD_DIRECTION ) {
            QList<QGraphicsItem *> items = selectedItems();
            if( !items.isEmpty() ) {
                ItemArrow *arrow = dynamic_cast<ItemArrow*>(items.at(0));
                if( arrow )
                    begin = mItemArrows.indexOf(arrow) + 1;
            }
        }
        for(int i=begin; i<mItemArrows.size(); ++i) {
            qDebug() << i;
            ItemArrow *arrow = mItemArrows.at(i);
            if( mFastFilter.match(*arrow->getTransaction(), _case) ) {
                clearSelection();
                arrow->setSelected(true);
                emit needCenter(arrow);
                return;
            }
        }
    } else {
        int begin = -1;
        QList<QGraphicsItem *> items = selectedItems();
        if( !items.isEmpty() ) {
            ItemArrow *arrow = dynamic_cast<ItemArrow*>(items.at(0));
            if( arrow )
                begin = mItemArrows.indexOf(arrow) - 1;
        }
        for(int i=begin; i>=0; --i) {
            ItemArrow *arrow = mItemArrows.at(i);
            if( mFastFilter.match(*arrow->getTransaction(), _case) ) {
                clearSelection();
                arrow->setSelected(true);
                emit needCenter(arrow);
                return;
            }
        }
    }
}

void Callflow::clearHighlight()
{
    mFastFilter = Filter();
}

void Callflow::hideComponent(const QString &_comp)
{
    mHiddenComponents << _comp;
    rebuild();
    update();
}

void Callflow::resetHidedComponents()
{
    mHiddenComponents.clear();
    rebuild();
    update();
}

void Callflow::setFilter(const QString &_filter) {
    qDebug() << "Changing transaction filter";
    mExpressionParser = ExpressionParser(_filter);
    rebuild();
    update();
}

void Callflow::reverse()
{
    std::reverse(mTransactions.begin(), mTransactions.end());
}

void Callflow::rebuild()
{
    qDebug() << "Updating" << mComponents.size() << mTransactions.size();
    qDeleteAll(mItemArrows);
    qDeleteAll(mItemComps);
    mItemArrows.clear();
    mItemComps.clear();
    QGraphicsScene::clear();

    // Components
    int addedComponents = 0;
    for(int i=0; i<mComponents.size(); ++i) {
        if( !mHiddenComponents.contains(mComponents.at(i)) ) {
            ItemComp *comp = new ItemComp(this, mComponents.at(i), mFont, TRANS_FIRST + TRANS_HEIGHT*10);
            comp->setPos(COMP_WIDTH * addedComponents, 0);
            addItem(comp);
            mItemComps.append(comp);
            connect(comp, SIGNAL(droppedComp(QString,QString)), SLOT(droppedComp(QString,QString)));
            ++addedComponents;
        }
    }

    // Transactions
    int addedTransactions = 0;
    for(int i=0; i<mTransactions.size(); ++i) {
        QPointer<Transaction> trans = mTransactions.at(i);
        if( mHiddenComponents.contains(trans->getSource().getComp()) || mHiddenComponents.contains(trans->getDest().getComp()) )
            continue;
        /// TLGLOG-4 filter transactions
        if( !mExpressionParser.match(*trans.data(), Qt::CaseInsensitive) )
            continue;
        {
            // get coordinats
            int src = 0;
            int dest = 0;
            for(int j=0; j<mItemComps.size(); ++j) {
                if(mItemComps[j]->title()==trans->getSource().getComp())
                    src = j;
                else if(mItemComps[j]->title()==trans->getDest().getComp())
                    dest = j;
            }
            Q_ASSERT(src>=0 && dest>=0);
            bool leftToRight = src < dest ? true : false;
            if( trans->isReply() )
                leftToRight = !leftToRight;
            int x = qMin(src, dest);
            int w = qAbs(src - dest);

            // get color
            QColor color(Qt::white);
            QString sessionId = trans->getSessionId();
            // TODO need to get session ID from reply if request hasn't it
            // issue is - there are no transaction reply in mTransaction
            if( sessionId.isEmpty() && !trans->isReply() ) {
            }
            if( !sessionId.isEmpty() ) {
                int index = mSessions.indexOf(sessionId);
                if( index==-1 ) {
                    index = mSessions.size();
                    mSessions << sessionId;
                }
                if( index==mSessionColors.size() )
                    mSessionColors << QColor(::random()%256, ::random()%256, ::random()%256);
                color = QColor( mSessionColors.at(index) );
            }

            // create
            ItemArrow *arrow = new ItemArrow(trans, leftToRight, w*COMP_WIDTH, color, mFont);
            arrow->setPos(x*COMP_WIDTH, addedTransactions*TRANS_HEIGHT + TRANS_FIRST);
            addItem(arrow);
            mItemArrows.append(arrow);

            // Draw timestamps if necessery
            TimestampImage *ts = new TimestampImage(trans->isReply() ? trans->getCompletedAsString() : trans->getIssuedAsString(), mFont);
            ts->setPos(-COMP_WIDTH/2, addedTransactions*TRANS_HEIGHT + TRANS_FIRST);
            addItem(ts);
            mTimestamps.append(ts);

            ++addedTransactions;
        }
    }

    // Update component's height
    int height = mItemArrows.size()*TRANS_HEIGHT + TRANS_FIRST;
    for( auto& comp : mItemComps) {
        comp->setHeight(height);
    }

    // Update views
    for(auto view : views() ) {
        view->matrix().reset();
        view->scale(1, 1);
    }
}

void Callflow::selectAll()
{
    clearSelection();
    for(int i=0; i<mItemArrows.size(); ++i)
        mItemArrows[i]->setSelected(true);
}

QString Callflow::exportSelectedTransactions(const QString &_template)
{
    qDebug() << "Exporing selected transactions with template" << _template;
    QString result;

    QStringList exportTemplate = _template.split("\n");
    auto items = selectedItems();
    auto orderedItems = QMap<int, ItemArrow*>();
    for(int i=0; i<items.size(); ++i) {
        ItemArrow *arrow = dynamic_cast<ItemArrow*>(items[i]);
        if( arrow ) {
            int idx = mItemArrows.indexOf(arrow);
            orderedItems[idx] = arrow;
        }
    }

    std::for_each(orderedItems.begin(), orderedItems.end(), [&result, &exportTemplate](ItemArrow* _it){
            result += _it->getTransaction()->exportTransaction(exportTemplate);
            result += "\n";
    } );

    return result;
}
