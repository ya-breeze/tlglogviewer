#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QPair>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QAbstractTableModel>

class ICallFlow;

#define DATETIME_FORMAT "ddd MMM d h:mm:ss yyyy"
#define DATETIME_LABEL "yyyy-MM-dd h:mm:ss"

/// Class to store one trasaction
class Transaction : public QAbstractTableModel
{
public:
    typedef QVector< QPair<QString, QString> > Ficses;

    struct Address {
        QString getComp() const;
        void setComp(const QString& _value);
        QString getFunc() const;
        void setFunc(const QString& _value);
        bool operator ==(const Address& _rh)const;
        QString toString() const;

    protected:
        QString mComp;
        QString mFunc;
    };

    Transaction(ICallFlow *_callflow);
    ICallFlow* callflow() const;
    void dump() const;

    QString        getType() const;
    void           setType(const QString& _value);
    const Address& getSource() const;
    Address&       getSource();
    void           setSource(const Address& _value);
    const Address& getDest() const;
    Address&       getDest();
    void           setDest(const Address& _value);
    int            getClass() const;
    void           setClass(int _value);
    QDateTime      getIssued() const;
    QString        getIssuedAsString() const;
    void           setIssued(const QString& _value, const QString& _format = DATETIME_FORMAT);
    QDateTime      getCompleted() const;
    QString        getCompletedAsString() const;
    void           setCompleted(const QString& _value, const QString& _format = DATETIME_FORMAT);
    int            getTransNumber() const;
    void           setTransNumber(int _value);
    int            getCc() const;
    void           setCc(int _value);
    int            getRsn() const;
    void           setRsn(int _value);
    int            getCompTransNumber() const;
    void           setCompTransNumber(int _value);

    void           addFics(const QString& _name, const QString& _value);

    /// \return session ID if present or empty string
    QString        getSessionId() const;
    /// \return name of FICS with session ID if present or empty string
    QString        getSessionIdFics() const;

    bool           isOk() const;
    bool           isReply() const;
    void           setReply(bool _value);
    /// \return text for ItemArrow
    QString        getArrowText() const;
    /// \return list of ficses with given name
    //QStringList    getFics(const QString& _name) const;
    /// \return first fics with given name
    QString        getFics(const QString& _name) const;

    /// \return true if any fics math given _value
    bool           match(const QString& _value, Qt::CaseSensitivity _case) const;

    /// \return true if given transaction is pair for this
    bool           isPair(const Transaction& _trans) const;

    QString exportTransaction(const QStringList& _exportTemplate);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
    Address    mSource;
    Address    mDest;
    QString    mType;
    int        mClass;
    QDateTime  mIssued;
    QDateTime  mCompleted;
    Ficses     mFicses;
    int        mTransNumb;
    int        mCompTransNumb;
    int        mCc;
    int        mRsn;
    bool       mIsReply;
    ICallFlow *pCallflow;
};

#endif // TRANSACTION_H
