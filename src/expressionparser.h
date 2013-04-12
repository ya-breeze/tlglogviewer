#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include <QString>

#include "transaction.h"

class ExpressionParserImpl;

class ExpressionParser
{
public:
    ExpressionParser(const QString& _filter);
    bool match(const Transaction& _trans, Qt::CaseSensitivity _case) const;

private:
    ExpressionParserImpl *pImpl;
};

#endif // EXPRESSIONPARSER_H
