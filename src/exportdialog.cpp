#include "exportdialog.h"
#include "ui_exportdialog.h"

#include <QFileDialog>
#include <QSettings>


#define DEFAULT_TEMPLATE "$FROMCOMP:$FROMFUNC=>$TOCOMP:$TOFUNC | $CC | $RSN | $ISSUED | $COMPLETED | $TRANSID | $ISREPLY \n*        $FICSNAME=\"$FICSVALUE\""

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
    connect((const QObject*)ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), SLOT(on_resetButton()));

    QSettings settings;
    QString tmpl = settings.value("export_template").toString();
    ui->te_Template->setText(!tmpl.isEmpty() ? tmpl : DEFAULT_TEMPLATE);
}

ExportDialog::~ExportDialog()
{
    QSettings settings;
    settings.setValue("export_template", getTemplate());

    delete ui;
}

QString ExportDialog::getTemplate() const
{
    return ui->te_Template->toPlainText();
}

void ExportDialog::on_toolButton_clicked()
{
    ui->le_Filename->setText( QFileDialog::getOpenFileName(this, tr("Choose file"), "", tr("Text Files (*.tlglog.txt *.txt)")) );
}

void ExportDialog::on_resetButton()
{
    ui->te_Template->setText(DEFAULT_TEMPLATE);
}
