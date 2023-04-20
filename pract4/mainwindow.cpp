#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "conversor.h"
#include <iostream>
#include <QValidator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

union Code {

    struct{
        unsigned int partFrac : 23;
        unsigned int expo : 8;
        unsigned int sign : 1;
    }bitfield;

    float numero;
    unsigned int numerox;
};


void MainWindow::on_pushPlus_clicked()
{
    Conversor conver;
    QString val1 = ui->textOp1Real->toPlainText();

    bool esNumero = false;
    float num1 = val1.toFloat(&esNumero);

    if (esNumero) {
        QString val2 = ui->textOp2Real->toPlainText();
        float num2 = val2.toFloat(&esNumero);
        if(esNumero){
            union Code a = conver.floattoIEE(num1);
            union Code b = conver.floattoIEE(num2);
        }else{
            std::cout << "mal2" << std::endl;
        }
    } else {
        std::cout << "mal" << std::endl;
    }





}

