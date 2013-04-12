#ifndef EXPRESSIONPARSERIMPL_H
#define EXPRESSIONPARSERIMPL_H

#include <stdexcept>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <QString>

#include "transaction.h"

namespace qi    = boost::spirit::qi;
namespace phx   = boost::phoenix;

struct op_or  {};
struct op_and {};
struct op_xor {};
struct op_not {};
struct op_equal  {};

typedef std::string var;
template <typename tag> struct binop;
template <typename tag> struct unop;

typedef boost::variant<var,
        boost::recursive_wrapper<unop <op_not> >,
        boost::recursive_wrapper<binop<op_and> >,
        boost::recursive_wrapper<binop<op_or> >
        > expr;

template <typename tag> struct binop
{
    explicit binop(const expr& l, const expr& r) : oper1(l), oper2(r) { }
    expr oper1, oper2;
};

template <typename tag> struct unop
{
    explicit unop(const expr& o) : oper1(o) { }
    expr oper1;
};

class ExpressionParserImpl {
public:
    ExpressionParserImpl(const QString& _filter);
    bool match(const Transaction& _trans, Qt::CaseSensitivity _case) const;

private:
    expr mExprTree;
};

struct ExpressionParserException : public std::runtime_error {
//    ExpressionParserException(const QString& _text) : std::runtime_error(_text.toStdString()) {}
    ExpressionParserException(const std::string& _text) : std::runtime_error(_text) {}
};

#endif // EXPRESSIONPARSERIMPL_H
