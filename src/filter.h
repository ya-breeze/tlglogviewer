#ifndef FILTER_H
#define FILTER_H


#include <QString>
#include <QMap>

#include "transaction.h"

class Filter
{
public:
    typedef QMap<QString, QString> FilterItems;
    Filter();
    Filter(const QString& _filter);
    bool match(const Transaction& _trans, Qt::CaseSensitivity _case) const;
    bool match(const QString& _ficsName, const QString& _ficsValue) const;
    QString getValue(const Transaction& _trans, const QString& _name) const;


    static bool match(const Transaction& _trans, const QString& _key, const QString& _value, Qt::CaseSensitivity _case);

private:
    FilterItems mFilterItems;
};

#endif // FILTER_H
