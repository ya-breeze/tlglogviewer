#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget *parent = 0);
    ~ExportDialog();
    QString getTemplate() const;
    
private slots:
    void on_toolButton_clicked();
    void on_resetButton();

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
