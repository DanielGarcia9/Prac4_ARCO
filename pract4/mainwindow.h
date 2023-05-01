#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void IEEHEX(union Code num, int pos);
     void REALHEX(union Code num);

private slots:
    void on_pushPlus_clicked();


    void on_pushMult_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
