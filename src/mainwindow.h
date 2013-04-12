#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "callflow.h"
#include "sortfiltermodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void load(const QString& _fileName);
    ~MainWindow();

protected slots:
    void moved(QMouseEvent *_event);
    void clicked(QPoint _event);
    void updateTransaction(ItemArrow *_arrow);
    void needCenter(QGraphicsItem*_arrow);

private slots:
    void onFindTextChanged(QString);

    void on_actionLoad_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionFind_triggered();

    void on_actionCopyFics();

    void on_btnNext_clicked();
    void on_btnPrev_clicked();
    void on_btnHide_clicked();

    void on_actionSelect_All_triggered();

    void on_action_Export_selected_triggered();

    void on_actionDrawTimestamps_triggered();

    void on_leFicsFilter_textChanged(const QString &_text);

    void on_btnFilter_clicked();

private:
    Ui::MainWindow *ui;
    Callflow       mCallFlow;
    SortFilterModel mProxyModel;
    /// Path for load file
    QString        mLastPath;
};

#endif // MAINWINDOW_H
