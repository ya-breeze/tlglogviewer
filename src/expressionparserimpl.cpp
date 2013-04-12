#include "expressionparserimpl.h"

#include "filter.h"

struct Checker : boost::static_visitor<void>
{
    Checker(const Transaction& _trans, Qt::CaseSensitivity _case)
        : refTrans(_trans), mCase(_case), mResult(true) {}

    bool result() const { return mResult; }

    void operator()(const var& v) {
        size_t idx = v.find('=');
        if( idx==v.npos )
            mResult = Filter::match(refTrans, QString::fromStdString(v), "", mCase);
        else
            mResult = Filter::match(refTrans, QString::fromStdString(v.substr(0, idx)),
                                    QString::fromStdString(v.substr(idx+1)), mCase);
    }

    void operator()(const binop<op_and>& b) {
        auto lChecker = Checker(refTrans, mCase);
        auto rChecker = Checker(refTrans, mCase);
        boost::apply_visitor(lChecker, b.oper1);
        boost::apply_visitor(rChecker, b.oper2);

        mResult = lChecker.result() && rChecker.result();
    }
    void operator()(const binop<op_or >& b) {
        auto lChecker = Checker(refTrans, mCase);
        auto rChecker = Checker(refTrans, mCase);
        boost::apply_visitor(lChecker, b.oper1);
        boost::apply_visitor(rChecker, b.oper2);

        mResult = lChecker.result() || rChecker.result();
    }
//    void operator()(const binop<op_xor>& b) {
//        mResult = true;
//    }

    void operator()(const unop<op_not>& u)
    {
        auto uChecker = Checker(refTrans, mCase);
        boost::apply_visitor(uChecker, u.oper1);
        mResult = !uChecker.result();
    }

private:
    const Transaction& refTrans;
    Qt::CaseSensitivity mCase;
    bool mResult;
};

//std::ostream& operator<<(std::ostream& os, const expr& e)
//{ boost::apply_visitor(printer(os), e); return os; }

template <typename It, typename Skipper = qi::space_type>
    struct parser : qi::grammar<It, expr(), Skipper>
{
    parser() : parser::base_type(expr_)
    {
        using namespace qi;

        expr_  = or_.alias();

        or_    = (and_ >> no_case["or"]  >> or_ )   [ _val = phx::construct<binop<op_or >>(_1, _2) ]  | and_   [ _val = _1 ];
        and_   = (not_ >> no_case["and"] >> and_)   [ _val = phx::construct<binop<op_and>>(_1, _2) ]  | not_   [ _val = _1 ];
        not_   = (no_case["not"] > simple       )   [ _val = phx::construct<unop <op_not>>(_1)     ]  | simple [ _val = _1 ];

        simple = (('(' > expr_ > ')') | var_);
        var_   = qi::lexeme[ +(alpha | digit | char_('$') | char_('=') ) ];

        BOOST_SPIRIT_DEBUG_NODE(expr_);
        BOOST_SPIRIT_DEBUG_NODE(or_);
        BOOST_SPIRIT_DEBUG_NODE(xor_);
        BOOST_SPIRIT_DEBUG_NODE(and_);
        BOOST_SPIRIT_DEBUG_NODE(not_);
        BOOST_SPIRIT_DEBUG_NODE(simple);
        BOOST_SPIRIT_DEBUG_NODE(var_);
        BOOST_SPIRIT_DEBUG_NODE(equal_);
    }

  private:
    qi::rule<It, var() , Skipper> var_;
    qi::rule<It, expr(), Skipper> not_, and_, xor_, or_, simple, expr_, equal_;
};

//int test()
//{
//    for (auto& input : std::list<std::string> {
//            // From the OP:
//            "(a and b) xor ((c and d) or (a and b));",
//            "a and b xor (c and d or a and b);",

//            /// Simpler tests:
//            "a and b;",
//            "a or b;",
//            "a xor b;",
//            "not a;",
//            "not a and b;",
//            "not (a and b);",
//            "a or b or c;",
//            "a and b and c or not a and not b and not c;",
//            "AAA=123 and BBB=222 and $REPLY;",
//            "AAA=123 AND BBB=222;",
//            })
//    {
//        auto f(std::begin(input)), l(std::end(input));
//        parser<decltype(f)> p;

//        try
//        {
//            expr result;
//            bool ok = qi::phrase_parse(f,l,p > ';',qi::space,result);

//            if (!ok)
//                std::cerr << "invalid input\n";
//            else
//                std::cout << "result: " << result << "\n";

//        } catch (const qi::expectation_failure<decltype(f)>& e)
//        {
//            std::cerr << "expectation_failure at '" << std::string(e.first, e.last) << "'\n";
//        }

//        if (f!=l) std::cerr << "unparsed: '" << std::string(f,l) << "'\n";
//    }

//    return 0;
//}


ExpressionParserImpl::ExpressionParserImpl(const QString &_filter) {
    std::string filter(_filter.toStdString());
    auto f(std::begin(filter)), l(std::end(filter));
    parser<decltype(f)> p;

    try
    {
        if( !qi::phrase_parse(f,l,p > ';', qi::space, mExprTree) )
            throw ExpressionParserException("Invalid input");
    } catch (const qi::expectation_failure<decltype(f)>& e) {
        std::stringstream ss;
        ss << "expectation_failure at '" << std::string(e.first, e.last) << "' in '" << filter << "'";
        throw ExpressionParserException(ss.str());
    }

    if (f!=l) {
        std::stringstream ss;
        ss << "unparsed: '" << std::string(f,l);
        throw ExpressionParserException(ss.str());
    }
}

bool ExpressionParserImpl::match(const Transaction &_trans, Qt::CaseSensitivity _case) const {
    auto checker = Checker(_trans, _case);
    boost::apply_visitor(checker, mExprTree);
    return checker.result();
}
