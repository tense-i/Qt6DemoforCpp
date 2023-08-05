#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QLabel;
class QStandardItemModel;
class QItemSelectionModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QLabel *labCurFile;
    QLabel *labCellPos;
    QLabel *labCellText;
    const int FixedColumnCount=6;
    QStandardItemModel *model;
    QItemSelectionModel *SelecModel;
    void initModelData(QStringList&aFileContent);
private slots:
    void do_currentChanged(const QModelIndex &current,const QModelIndex &prev);
    void on_act_openFile_triggered();

    void on_act_ShowData_triggered();

    void on_act_AddRow_triggered();

    void on_act_InsertRow_triggered();

    void on_act_deleteRow_triggered();

    void on_act_Center_triggered();

    void on_act_left_triggered();

    void on_act_Borld_triggered(bool checked);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
