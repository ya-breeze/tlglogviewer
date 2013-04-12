#include "expressionparser.h"

#include "expressionparserimpl.h"

ExpressionParser::ExpressionParser(const QString &_filter)
    : pImpl(NULL)
{
    if( _filter.isEmpty() )
        return;

    pImpl = new ExpressionParserImpl(_filter+";");
}

bool ExpressionParser::match(const Transaction &_trans, Qt::CaseSensitivity _case) const {
    if( !pImpl )
        return true;

    return pImpl->match(_trans, _case);
}
