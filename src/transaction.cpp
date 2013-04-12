#include "transaction.h"

#include <QDebug>
#include <QBrush>

#include "ICallFlow.h"

Transaction::Transaction(ICallFlow *_callflow)
    : QAbstractTableModel(), mClass(0), mTransNumb(0), mCompTransNumb(0),
      mCc(0), mRsn(0), mIsReply(false), pCallflow(_callflow)
{
}

ICallFlow *Transaction::callflow() const
{
    return pCallflow;
}

void Transaction::dump() const
{
    qDebug() << "Transaction "
             << getSource().getComp() << ":" << getSource().getFunc()
             << " => "
             << getDest().getComp() << ":" << getDest().getFunc()
                ;
    qDebug() << "    CLASS     : " << mClass;
    qDebug() << "    ISSUED    : " << mIssued;
    qDebug() << "    COMPLETED : " << mCompleted;


    for(int i=0; i<mFicses.size(); ++i)
        qDebug() << "    " << mFicses.at(i).first << "=" << mFicses.at(i).second;
}

QString Transaction::getType() const
{
    return mType;
}

void Transaction::setType(const QString &_value)
{
    mType = _value;
}

const Transaction::Address& Transaction::getSource() const
{
    return mSource;
}

Transaction::Address &Transaction::getSource()
{
    return mSource;
}

void Transaction::setSource(const Address& _value)
{
    mSource = _value;
}

const Transaction::Address &Transaction::getDest() const
{
    return mDest;
}

Transaction::Address &Transaction::getDest()
{
    return mDest;
}

void Transaction::setDest(const Address& _value)
{
    mDest = _value;
}

int Transaction::getClass() const
{
    return mClass;
}

void Transaction::setClass(int _value)
{
    mClass = _value;
}

QDateTime Transaction::getIssued() const
{
    return mIssued;
}

QString Transaction::getIssuedAsString() const
{
    return mIssued.toString(DATETIME_LABEL);
}

void Transaction::setIssued(const QString &_value, const QString& _format)
{
    mIssued = QDateTime::fromString(_value.simplified(), _format);
}

QDateTime Transaction::getCompleted() const
{
    return mCompleted;
}

QString Transaction::getCompletedAsString() const
{
    return mCompleted.toString(DATETIME_LABEL);
}

void Transaction::setCompleted(const QString &_value, const QString& _format)
{
    mCompleted = QDateTime::fromString(_value.simplified(), _format);
    mIsReply = mCompleted.isValid();
}

int Transaction::getTransNumber() const
{
    return mTransNumb;
}

void Transaction::setTransNumber(int _value)
{
    mTransNumb = _value;
}

int Transaction::getCompTransNumber() const
{
    return mCompTransNumb;
}

void Transaction::setCompTransNumber(int _value)
{
    mCompTransNumb = _value;
}

int Transaction::getCc() const
{
    return mCc;
}

void Transaction::setCc(int _value)
{
    mCc = _value;
}

int Transaction::getRsn() const
{
    return mRsn;
}

void Transaction::setRsn(int _value)
{
    mRsn = _value;
}

void Transaction::addFics(const QString &_name, const QString &_value)
{
    mFicses << QPair<QString, QString>(_name, _value);
}

QString Transaction::getSessionId() const
{
    return getFics(getSessionIdFics()).trimmed();
}

QString Transaction::getSessionIdFics() const
{
    QString result;
    // TODO Move into config file
    if( getSource().getComp().startsWith("SAI") || getDest().getComp().startsWith("SAI") ) {
        result = "SESSION";
        QString value = getFics(result);
        if( value.isEmpty() )
            result = "_REF_SESS";
    }
    else if( getSource().getComp().startsWith("DIG") || getDest().getComp().startsWith("DIG") )
        result = "SESSION";
    else if( getSource().getComp().startsWith("SCC") || getDest().getComp().startsWith("SCC") )
        result = "SESSION";
    else if( getSource().getComp().startsWith("MBL") || getDest().getComp().startsWith("MBL") )
        result = "SESSION";
    else if( getSource().getComp().startsWith("SSI") || getDest().getComp().startsWith("SSI") ) {
        result = "REF_SESS";
        QString value = getFics(result);
        if( value.isEmpty() ) {
            result = "CALL";
            value = getFics(result);
        }
        if( value.isEmpty() )
            result = "SESSION";
    }
    else if( getSource().getComp().startsWith("CCS") || getDest().getComp().startsWith("CCS") )
        result = "CALL";
    else if( getSource().getComp().startsWith("GAP") || getDest().getComp().startsWith("GAP") )
        result = "SESSION";

    return result.trimmed();
}

bool Transaction::isOk() const
{
    return (getCc()==0 && getRsn()==0);
}

bool Transaction::isReply() const
{
    return mIsReply;
}

QString Transaction::getArrowText() const
{
    QString result = getDest().getFunc();
    if( isReply() )
        result += " reply";

    // TODO Move into config file
    if( getDest().getFunc()=="REXC" )
        result += ":" + getFics("EXECNAME");
    else if( getDest().getFunc()=="EXEC" )
        result += ":" + getFics("EXECNAME");
    else if( !isReply() && getDest().getFunc()=="SUPR" && getDest().getComp().startsWith("SAI") )
        result += ":" + getFics("MSG_TYPES");
    else if( isReply() && getDest().getFunc()=="SUPR" && getDest().getComp().startsWith("SAI") )
        result += ":" + getFics("S_MSG_TYPE");
    else if( getDest().getComp().startsWith("DTA") && getDest().getFunc()=="QUER" )
        result += ":" + getFics("QNAME");
    else if( getDest().getFunc()=="LOAD" || getDest().getFunc()=="SAVE" )
        result += ":" + getFics("STYPE") + ":" + getFics("SID");

    return result;
}

QString Transaction::getFics(const QString &_name) const
{
    for(int i=0; i<mFicses.size(); ++i)
        if( mFicses.at(i).first==_name )
            return mFicses.at(i).second;

    return "";
}

bool Transaction::match(const QString &_value, Qt::CaseSensitivity _case) const
{
    if(mSource.getComp().contains(_value, _case)
            || mSource.getFunc().contains(_value, _case)
            || mDest.getComp().contains(_value, _case)
            || mDest.getFunc().contains(_value, _case)
            ) return true;
    for(int i=0; i<mFicses.size(); ++i) {
        const QPair<QString, QString>& fics = mFicses.at(i);
        QString value = fics.first + "=" + fics.second;
        if(value.contains(_value, _case))
            return true;
    }

    return false;
}

bool Transaction::isPair(const Transaction &_trans) const
{
    qDebug() << getSource().toString() << "=>" << getDest().toString() << "vs" <<_trans.getSource().toString() << "=>" << _trans.getDest().toString()
             << ": " << (_trans.getSource()==getSource()) << (_trans.getDest()==getDest()) << (_trans.getIssued()==getIssued())
             << _trans.getIssued() << "-" << getIssued();
    if( !(_trans.getSource()==getSource()) )
        return false;
    if( !(_trans.getDest()==getDest()) )
        return false;
    if( _trans.getIssued()!=getIssued() )
        return false;

    return true;
}

QString Transaction::exportTransaction(const QStringList &_exportTemplate)
{
    QString result;
    for(int i=0; i<_exportTemplate.size(); ++i) {
        QString line = _exportTemplate[i];
        line.replace("$FROMCOMP", mSource.getComp());
        line.replace("$FROMFUNC", mSource.getFunc());
        line.replace("$TOCOMP", mDest.getComp());
        line.replace("$TOFUNC", mDest.getFunc());
        line.replace("$CC", QString::number(mCc));
        line.replace("$RSN", QString::number(mRsn));
        line.replace("$ISSUED", mIssued.toString(DATETIME_LABEL));
        line.replace("$COMPLETED", mCompleted.isValid() ? mCompleted.toString(DATETIME_LABEL) : "<NOT COMPLETED>");
        line.replace("$TRANSID", QString::number(mTransNumb));
        line.replace("$ISREPLY", QString::number(mIsReply));

        if(!line.isEmpty() && line.at(0)=='*') {
            // Add line for every FICS
            line.remove(0, 1);
            QString tmpl = line;
            for(int j=0; j<mFicses.size(); ++j) {
                line.replace("$FICSNAME", mFicses[j].first);
                line.replace("$FICSVALUE", mFicses[j].second);
                result += line;
                result += "\n";
                line = tmpl;
            }
        } else {
            result += line;
            result += "\n";
        }
    }

    return result;
}

int Transaction::rowCount(const QModelIndex &/*parent*/) const
{
    return mFicses.size();
}

int Transaction::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

QVariant Transaction::data(const QModelIndex &index, int role) const
{
    if( role==Qt::BackgroundRole ) {
        // Highlight session fics with yellow
        if( mFicses.at(index.row()).first==getSessionIdFics() ) {
            return QBrush( QColor(Qt::yellow) );
        }
        // Highlight fast-search text
        const QPair<QString, QString>& fics = mFicses.at(index.row());

        if( pCallflow->getFastFind().match(fics.first, fics.second))
            return QBrush( QColor(255, 0, 0, 50) );

        return QVariant();
    } else if( role!=Qt::DisplayRole && role!=Qt::EditRole )
        return QVariant();

    if( index.column())
        return mFicses.at(index.row()).second;
    else
        return mFicses.at(index.row()).first;
}

QVariant Transaction::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role!=Qt::DisplayRole )
        return QVariant();

    if( orientation!=Qt::Horizontal )
        return QVariant();

    if( section )
        return "VALUE";
    else
        return "FICS";
}

Qt::ItemFlags Transaction::flags(const QModelIndex &/*index*/) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QString Transaction::Address::getComp() const
{
    return mComp;
}

void Transaction::Address::setComp(const QString &_value)
{
    mComp = _value;
}

QString Transaction::Address::getFunc() const
{
    return mFunc;
}

void Transaction::Address::setFunc(const QString &_value)
{
    mFunc = _value;
}

bool Transaction::Address::operator ==(const Transaction::Address &_rh) const
{
    if( getComp()!=_rh.getComp() )
        return false;

    return getFunc()==_rh.getFunc();
}

QString Transaction::Address::toString() const
{
    return mComp + ":" + mFunc;
}

void Transaction::setReply(bool _value)
{
    mIsReply = _value;
}
