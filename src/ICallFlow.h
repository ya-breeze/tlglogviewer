#ifndef ICALLFLOW_H
#define ICALLFLOW_H

#include <QString>
#include <QPointer>

#include "itemarrow.h"
#include "filter.h"

struct ICallFlow {
    virtual void    selectPairTransaction(const ItemArrow *_arrow) = 0;
    virtual void    selectSession(const QString& _sessionId) = 0;
    virtual bool    needDrawTimestamp() const = 0;
    virtual const Filter& getFastFind() const = 0;

    /// Hide given component
    virtual void hideComponent(const QString& _comp) = 0;
    /// Reset filters - draw all transactions/components
    virtual void resetHidedComponents() = 0;
};

#endif // ICALLFLOW_H
