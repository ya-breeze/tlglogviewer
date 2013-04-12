#include "filter.h"


#include <QStringList>
#include <QDebug>

Filter::Filter() {
}

Filter::Filter(const QString &_filter) {
    QStringList items = _filter.split("&&");
    for(const auto& item : items) {
        QString tmp = item.trimmed();
        int idx = tmp.indexOf("=");
        QString name = tmp;
        QString value;
        if( idx!=-1 ) {
            name = tmp.left(idx).trimmed();
            value = tmp.mid(idx+1).trimmed();
        }
        if( name.isEmpty() )
            continue;
        mFilterItems[name] = value;
    }
}

bool Filter::match(const Transaction& _trans, const QString& _key, const QString& _value, Qt::CaseSensitivity _case) {
    if( _key=="$SESSION" ) {
        if( !_trans.getSessionId().contains(_value, _case) )
            return false;
    } else if( _key=="$FROMCOMP" ) {
        if( !_trans.getSource().getComp().contains(_value, _case) )
            return false;
    } else if( _key=="$TOCOMP" ) {
        if( !_trans.getDest().getComp().contains(_value, _case) )
            return false;
    } else if( _key=="$TOFUNC" ) {
        if( !_trans.getDest().getFunc().contains(_value, _case) )
            return false;
    } else if( _key=="$CC" ) {
        if( QString::number(_trans.getCc())!=_value )
            return false;
    } else if( _key=="$RSN" ) {
        if( QString::number(_trans.getRsn())!=_value )
            return false;
    } else if( (_key=="$COMPLETED" || _key=="$REPLY" || _key=="$ISREPLY") ) {
        if( !_trans.getCompleted().isValid() )
            return false;
    } else if( _key=="$TRANSID" ) {
        if( QString::number(_trans.getTransNumber())!=_value )
            return false;
    } else {
        QString fics = _key;
        if( !_value.isEmpty() )
            fics += "=" + _value;
        if( !_trans.match(fics, _case) )
            return false;
    }

    return true;
}


bool Filter::match(const Transaction &_trans, Qt::CaseSensitivity _case) const {
    for(QMap<QString, QString>::const_iterator it = mFilterItems.constBegin(); it!=mFilterItems.constEnd(); ++it) {
        if( !match(_trans, it.key(), it.value(), _case) )
            return false;
    };

    return true;
}

bool Filter::match(const QString &_ficsName, const QString &_ficsValue) const {
    for(QMap<QString, QString>::const_iterator it = mFilterItems.constBegin(); it!=mFilterItems.constEnd(); ++it) {
        // TODO Refactor this ugly code )
        if( it.key().at(0)!='$' ) {
            if( !it.value().isEmpty() )
                if( _ficsName.contains(it.key()) || _ficsValue.contains(it.key()))
                    return true;

            if( _ficsName.contains(it.key()) && _ficsValue.contains(it.value()))
                return true;
        }
    };

    return false;
}
