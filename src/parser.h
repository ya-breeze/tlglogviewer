#ifndef PARSER_H
#define PARSER_H

#include "callflow.h"

class Parser
{
    enum TlgFormats {
        WORKSTATION,
        P90CTLG
    };

public:
    Parser();
    void fill(const QString& _fname, Callflow& _callflow);

protected:
    void parseWorkstation(const QString& _fname, Callflow& _callflow);
    void parseCtlg(const QString& _fname, Callflow& _callflow);
};

#endif // PARSER_H
