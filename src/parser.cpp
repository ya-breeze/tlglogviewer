#include "parser.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>

Parser::Parser()
{
}

void Parser::fill(const QString &_fname, Callflow &_callflow)
{
    QFile file(_fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString line = file.readLine();
    if( line.isEmpty() )
        return;

    if( line.startsWith("# *Sender* *Destination* *Function* *Completion code* *Reason code*") )
        parseWorkstation(_fname, _callflow);
    else
        parseCtlg(_fname, _callflow);

    _callflow.rebuild();
}

void Parser::parseWorkstation(const QString &_fname, Callflow &_callflow)
{
    qDebug() << "Parsing workstation file" << _fname;

    _callflow.clear();

    QFile file(_fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while (!file.atEnd()) {
        QString line = file.readLine();
        QString trimmed = line.trimmed();
        if( trimmed.isEmpty() || trimmed.at(0)=='#' )
            continue;

        QPointer<Transaction> trans = QPointer<Transaction>(new Transaction(&_callflow));

        // header
        QStringList values = line.split(" ", QString::SkipEmptyParts);
        if( values.size()<14 ) {
            QMessageBox::critical(NULL, "Invalid format", "Callflow can be broken. Too short line '" + line.trimmed() + "'");
            return;
        }
        int index = 0;
        trans->getSource().setComp(values.at(index++));
        trans->getDest().setComp(values.at(index++));
        trans->getDest().setFunc(values.at(index++));
        trans->setCc(values.at(index++).toInt());
        trans->setRsn(values.at(index++).toInt());
        trans->setTransNumber(values.at(index++).toInt());
        trans->setCompTransNumber(values.at(index++).toInt());

        QString date;
        for(int i=7; i<11; ++i)
            date += values.at(i) + " ";
        date += values.at(12);
        trans->setIssued( date );

        if( values.size()>=21 ) {
            date = "";
            for(int i=13; i<17; ++i)
                date += values.at(i) + " ";
            date += values.at(18);
            trans->setCompleted( date );

            index = 19;
        } else {
            index = 13;
        }
        trans->setReply(values.at(index)=="Reply");

        // ficses
        while( !line.isEmpty() ) {
            line = file.readLine();
            trimmed = line.trimmed();
            if( trimmed.isEmpty() ) {
                _callflow.addTransaction(trans);
                break;
            }

            int idx = line.indexOf("=");
            trans->addFics(line.left(idx).trimmed(),
                           line.mid(idx+1, line.length()-idx-2).replace("\\r\\n", "\n"));
        }
    }

    // WS logs are mostly reversed
    if( _callflow.sizeTransactions()>1
        && _callflow.getTransaction(0)->getIssued()>=
            _callflow.getTransaction(_callflow.sizeTransactions()-1)->getIssued() )
        _callflow.reverse();
}

void Parser::parseCtlg(const QString &_fname, Callflow &_callflow)
{
    qDebug() << "Parsing ctlg file" << _fname;

    _callflow.clear();

    QFile file(_fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    bool insideTransaction = false;
    bool insideFicses      = false;
    QString ficsName, ficsValue;
    QPointer<Transaction> trans = QPointer<Transaction>(new Transaction(&_callflow));
    QString line = file.readLine();
    while (!file.atEnd()) {
        if( !insideTransaction ) {
            if( line.startsWith("TYPE") ) {
                insideTransaction = true;
                insideFicses = false;
                trans = QPointer<Transaction>(new Transaction(&_callflow));
            }
        }
        if( !insideFicses ) {
            QStringList values = line.split(" : ");
            QString key = values.at(0).trimmed();
            QString value = values.size()>1 ? values.at(1).trimmed() : "";
            if( key=="TYPE" )
                trans->setType(value);
            else if( key=="DEST")
                trans->getDest().setComp(value);
            else if( key=="DEST FUNC")
                trans->getDest().setFunc(value);
            else if( key=="RET DEST")
                trans->getSource().setComp(value);
            else if( key=="RET FUNC")
                trans->getSource().setFunc(value);
            else if( key=="CLASS")
                trans->setClass(value.toInt());
            else if( key=="ISSUED")
                trans->setIssued(value);
            else if( key=="COMPLETED")
                trans->setCompleted(value);
            else if( key=="RSN")
                trans->setRsn(value.toInt());
            else if( key=="CC")
                trans->setCc(value.toInt());
            else if( key=="FICS" ) {
                insideFicses = true;
                QStringList valuesFics = values.at(1).split(" = ");
                ficsName  = valuesFics[0];
                ficsValue = valuesFics.size()>1 ? valuesFics[1] : "";
            } else if( key.isEmpty() && !trans->getType().isEmpty() ) {
                // No ficses in this transaction
                qDebug() << "transaction is over";
                insideTransaction = false;
                insideFicses = false;
                trans->dump();
                _callflow.addTransaction(trans);
            }
        } else {
            // FICSES
            if( line.trimmed().size()==0 ) {
                while( line.length()>0 && line.trimmed().size()==0 ) {
                    ficsValue += line;
                    line = file.readLine();
                }
            }
            if( line.startsWith("TYPE") || line.startsWith("  CHECK TLG LOG FILES FINISHED!") ) {
                // transaction is over
                qDebug() << "transaction is over";
                insideTransaction = false;
                insideFicses = false;
                trans->addFics(ficsName.trimmed(), ficsValue.remove(ficsValue.length()-1, 1).trimmed());
                trans->dump();
                _callflow.addTransaction(trans);
                continue; // to prevent one more readline
            } else if( line.startsWith("                ")){
                trans->addFics(ficsName.trimmed(), ficsValue.remove(ficsValue.length()-1, 1).trimmed());
                QStringList values = line.split(" = ");
                ficsName  = values[0].trimmed();
                ficsValue = values.size()>1 ? values[1] : "";
            } else {
                ficsValue += line;
            }
        }

        line = file.readLine();
    }

    qDebug() << "Components " << _callflow.sizeComponents()
             << ", transactions " << _callflow.sizeTransactions();
}
