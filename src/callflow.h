#ifndef CALLFLOW_H
#define CALLFLOW_H

#include <QFont>
#include <QVector>
#include <QPointer>
#include <QKeyEvent>
#include <QStringList>
#include <QGraphicsScene>

#include "transaction.h"
#include "itemarrow.h"
#include "itemcomp.h"
#include "ICallFlow.h"
#include "timestampimage.h"
#include "filter.h"
#include "expressionparser.h"

#define COMP_WIDTH 300
#define TRANS_HEIGHT 50
#define TRANS_FIRST 100

class Callflow : public QGraphicsScene, public ICallFlow
{
    Q_OBJECT

public:
    enum FindDirection {
        NO_DIRECTION,
        FORWARD_DIRECTION,
        REVERSE_DIRECTION
    };

    Callflow();
    int sizeTransactions() const;
    QPointer<Transaction> getTransaction(int _index) const;
    void clear();
    void addTransaction(const QPointer<Transaction>& _value);
    void reverse();
    /// Rebuild graphic items
    void rebuild();

    void selectAll();
    QString exportSelectedTransactions(const QString& _template);

    int sizeComponents() const;
//    QString getComponent(int _index) const;


    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *_event);
    void keyPressEvent(QKeyEvent *event);


    void setFastFind(const QString& _filter);
    virtual const Filter& getFastFind() const;
    void selectPairTransaction(const ItemArrow *_arrow);
    void selectSession(const QString& _sessionId);
    bool needDrawTimestamp() const;
    void setNeedDrawTimestamp(bool _value);

public slots:
    /// Searching first arrow with fics matching _value
    void find(Qt::CaseSensitivity _case, FindDirection _direction);
    void clearHighlight();
    /// Hide given component
    void hideComponent(const QString& _comp);
    /// Reset filters - draw all transactions/components
    void resetHidedComponents();
    /// TLGLOG-4 Set filter for whole transactions
    void setFilter(const QString& _filter);

signals:
    void transactionSelected(ItemArrow*);
    void needCenter(QGraphicsItem*);

protected slots:
    void itemSelected();
    void droppedComp(const QString& _compDroppedOn, const QString& _compDropped);
    void delayedDroppedComp();



protected:
    typedef QVector< QPointer<Transaction> > TransactionVector;
    typedef QVector<ItemArrow*>  ItemArrowVector;
    typedef QVector<ItemComp*>   ItemCompVector;
    typedef QVector<TimestampImage*>  TimestampImageVector;

    void addComponent(const QString &_name);
    bool isCtrlPressed() const;

    QFont             mFont;
    QList<QColor>     mSessionColors;

    TransactionVector mTransactions;
    QStringList       mComponents;
    ItemArrowVector   mItemArrows;
    ItemCompVector    mItemComps;
    TimestampImageVector mTimestamps;
    QStringList       mSessions;

    QString           mCompDropped;
    QString           mCompDroppedOn;

    /// If true - transaction timestamp will be printed on the left
    bool              mNeedDrawTimestamps;

    /// Components that must be hidden
    QStringList       mHiddenComponents;

    Filter            mFastFilter;
    ExpressionParser  mExpressionParser;
};

#endif // CALLFLOW_H
