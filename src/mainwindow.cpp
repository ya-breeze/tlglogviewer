#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QMouseEvent>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QClipboard>

#include "parser.h"
#include "itemarrow.h"
#include "itemcomp.h"
#include "exportdialog.h"
#include "filter.h"

#define TLGLOG_NAME tr("TlglogViewer")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->gView->setScene(&mCallFlow);
    ui->frameFind->hide();

//    connect(ui->gView, SIGNAL(mouseMove(QMouseEvent*)), SLOT(moved(QMouseEvent*)));
    connect(&mCallFlow, SIGNAL(transactionSelected(ItemArrow*)), SLOT(updateTransaction(ItemArrow*)));
    connect(&mCallFlow, SIGNAL(needCenter(QGraphicsItem*)), SLOT(needCenter(QGraphicsItem*)));
    connect(ui->leFind, SIGNAL(textChanged(QString)), SLOT(onFindTextChanged(QString)));

    ui->tblFicses->setModel(&mProxyModel);

    // Add context menu to copy FICS
    {
        QAction *copyFics = new QAction("Copy FICS", this);
        connect(copyFics, SIGNAL(triggered()), SLOT(on_actionCopyFics()));
        ui->tblFicses->addAction(copyFics);
    }

    // Draw timestamps menu item
    ui->actionDrawTimestamps->setChecked(mCallFlow.needDrawTimestamp());

    QCoreApplication::setOrganizationName("Teligent");
    QCoreApplication::setOrganizationDomain("teligent.ru");
    QCoreApplication::setApplicationName("tlglogviewer");
}

void MainWindow::load(const QString &_fileName)
{
    mLastPath = _fileName;

    Parser parser;
    parser.fill(_fileName, mCallFlow);
    qreal s = ui->gView->width()/( mCallFlow.width()*1.2 );
    ui->gView->scale(s, s);

    setWindowTitle(TLGLOG_NAME + " - " + _fileName);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moved(QMouseEvent *_event) {
    std::cout << "move event " << ui->gView->mapToScene(_event->pos()).x() << ":" << ui->gView->mapToScene(_event->pos()).y() << std::endl;
}

void MainWindow::clicked(QPoint /*_event*/) {
    std::cout << "click event" << std::endl;
}

void MainWindow::updateTransaction(ItemArrow *_arrow)
{
    qDebug() << "Updating transaction info";
    QPointer<Transaction> transaction = _arrow ? _arrow->getTransaction() : NULL;
    mProxyModel.setSourceModel(transaction.data());
    ui->tblFicses->resizeColumnsToContents();
    ui->tblFicses->resizeRowsToContents();

    if( _arrow ) {
        ui->lblDest->setText(transaction->getDest().getComp());
        ui->lblDestFunc->setText(transaction->getDest().getFunc());
        ui->lblSource->setText(transaction->getSource().getComp());
        ui->lblSourceFunc->setText(transaction->getSource().getFunc());
        ui->lblOk->setText(transaction->isOk() ? "True" : "False");
        ui->lblCc->setText( QString::number(transaction->getCc()) );
        ui->lblRsn->setText( QString::number(transaction->getRsn()) );
        ui->lblIssued->setText(transaction->getIssuedAsString());
        ui->lblCompleted->setText(transaction->getCompletedAsString());
        ui->lblTransId->setText( QString::number(transaction->getTransNumber()) );
        ui->lblCompTransId->setText( QString::number(transaction->getCompTransNumber()) );

        ui->lblOk->setForegroundRole( transaction->isOk() ? QPalette::WindowText : QPalette::LinkVisited );

        ui->gView->ensureVisible(_arrow);
    } else {
        ui->lblDest->setText("<empty>");
        ui->lblDestFunc->setText("<empty>");
        ui->lblSource->setText("<empty>");
        ui->lblSourceFunc->setText("<empty>");
        ui->lblOk->setText("<empty>");
        ui->lblCc->setText("<empty>");
        ui->lblRsn->setText("<empty>");
        ui->lblIssued->setText("<empty>");
        ui->lblCompleted->setText("<empty>");
        ui->lblTransId->setText("<empty>");
        ui->lblCompTransId->setText("<empty>");

        ui->lblOk->setForegroundRole( QPalette::WindowText );
    }
}

void MainWindow::needCenter(QGraphicsItem *_arrow)
{
    ui->gView->centerOn(_arrow);
}

void MainWindow::on_actionLoad_triggered()
{
    qDebug() << "Loading";
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open tlglog"), mLastPath, tr("TlgLog Files (*.tlg *.tlglog.txt *.txt)"));
    if( fileName.isEmpty() )
        return;

    load(fileName);
}

void MainWindow::on_actionExit_triggered()
{
    qDebug() << "Quit";
    exit(0);
}

void MainWindow::on_actionAbout_triggered()
{
    qDebug() << "About";
    QMessageBox::about(this, "Tlg Log viewer", QString("Teligent tlglog viewer\nVersion: ")+QString(VERSION)+"\nCreated by Ilya Korolev <ilya.korolev@teligent.ru>");
}

void MainWindow::on_actionFind_triggered()
{
    qDebug() << "Finding...";
    ui->frameFind->show();
    ui->leFind->setFocus();
}

void MainWindow::on_actionCopyFics()
{
    qDebug() << "Copying current FICS to clipboard";
    QItemSelectionModel *selectionModel = ui->tblFicses->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if( selected.isEmpty() )
        return;
    QClipboard *clipboard = QApplication::clipboard();
    QString value;

    for(QModelIndex idx : selected) {
        if( !value.isEmpty() )
            value += "\n";
        QModelIndex ficsName  = ui->tblFicses->model()->index(idx.row(), 0);
        QModelIndex ficsValue = ui->tblFicses->model()->index(idx.row(), 1);
        value += ui->tblFicses->model()->data(ficsName).toString() + "=" + ui->tblFicses->model()->data(ficsValue).toString();
    }

    clipboard->setText(value);
}

void MainWindow::onFindTextChanged(QString _value)
{
    qDebug() << "find text is changed" << _value;
    if( ui->cbSearchOnEdit->isChecked() ) {
        mCallFlow.setFastFind(_value);
        mCallFlow.find(ui->cbCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive, Callflow::NO_DIRECTION );
    }
}

void MainWindow::on_btnNext_clicked()
{
    qDebug() << "find NEXT text";
    mCallFlow.setFastFind(ui->leFind->text());
    mCallFlow.find(ui->cbCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive, Callflow::FORWARD_DIRECTION );
}

void MainWindow::on_btnPrev_clicked()
{
    qDebug() << "find PREVIOUS text";
    mCallFlow.setFastFind(ui->leFind->text());
    mCallFlow.find(ui->cbCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive, Callflow::REVERSE_DIRECTION );
}

void MainWindow::on_btnHide_clicked()
{
    ui->frameFind->hide();
    mCallFlow.clearHighlight();
    ui->tblFicses->reset();
}

void MainWindow::on_actionSelect_All_triggered()
{
    mCallFlow.selectAll();
}

void MainWindow::on_action_Export_selected_triggered()
{
    qDebug() << "exporting transactions";

    // Get export parameters
    ExportDialog dlg;
    if(dlg.exec()==QDialog::Rejected)
        return;

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(mCallFlow.exportSelectedTransactions(dlg.getTemplate()));
}

void MainWindow::on_actionDrawTimestamps_triggered()
{
    mCallFlow.setNeedDrawTimestamp(ui->actionDrawTimestamps->isChecked());
}

void MainWindow::on_leFicsFilter_textChanged(const QString &_text)
{
    mProxyModel.setFilterRegExp(_text);
}

void MainWindow::on_btnFilter_clicked() {
    mCallFlow.setFilter(ui->leFilter->text());
}
