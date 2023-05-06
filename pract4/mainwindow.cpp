#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "conversor.h"
#include "alu.h"
#include <iostream>
#include <bitset>
#include <sstream>
#include <cmath>

using namespace std;

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

Conversor conver;
ALU alu;


void MainWindow::on_pushPlus_clicked() //SUMA
{

    QString val1 = ui->textOp1Real->toPlainText();
    QString val2 = ui->textOp2Real->toPlainText();

    bool esNumero = false;
    bool esNumero2 = false;
    float num1 = val1.toFloat(&esNumero);
    float num2 = val2.toFloat(&esNumero2);

    if (esNumero && esNumero2) {
        union Code a = conver.floattoIEE(num1);
        union Code b = conver.floattoIEE(num2);
        IEEHEX(a, 1);
        IEEHEX(b, 2);

        union Code result = alu.suma(a, b);

        bitset<1> sign(result.bitfield.sign);
        bitset<8> exp(result.bitfield.expo);
        bitset<23> frac(result.bitfield.partFrac);

        bitset<32> resultado((sign.to_ulong() << 31) | (exp.to_ulong() << 23) | frac.to_ulong());

        string re = resultado.to_string();
        ui->textBi->setText(QString::fromStdString(re));

        REALHEX(result);

    }else{
        if(isinf(num2) && isinf(num1)){
            union Code a;
            if(num1 > 0){
                a.bitfield.sign = 0;
            }else{
                a.bitfield.sign = 1;
            }
            a.bitfield.expo = 255;
            a.bitfield.partFrac = 0;
            IEEHEX(a, 1);
            union Code b;
            if(num2 > 0){
                b.bitfield.sign = 0;
            }else{
                b.bitfield.sign = 1;
            }
            b.bitfield.expo = 255;
            b.bitfield.partFrac = 0;
            IEEHEX(b, 2);
            if(num1 == -num2){
                union Code result;
                result.numero = 0;

                bitset<1> sign(result.bitfield.sign);
                bitset<8> exp(result.bitfield.expo);
                bitset<23> frac(result.bitfield.partFrac);

                bitset<32> resultado((sign.to_ulong() << 31) | (exp.to_ulong() << 23) | frac.to_ulong());

                string re = resultado.to_string();
                ui->textBi->setText(QString::fromStdString(re));

                REALHEX(result);

            }else{
                union Code res;
                if(num2 < 0){
                    res.bitfield.sign = 1;
                    ui->textBi->setText("11111111100000000000000000000000");
                }else{
                    res.bitfield.sign = 0;
                    ui->textBi->setText("01111111100000000000000000000000");
                }
                res.bitfield.expo = 255;
                res.bitfield.partFrac = 0;
                REALHEX(res);
            }
        }else if(isinf(num2) && esNumero){
            union Code a = conver.floattoIEE(num1);
            IEEHEX(a, 1);
            union Code b;
            if(num1 > 0){
                b.bitfield.sign = 0;
            }else{
                b.bitfield.sign = 1;
            }
            b.bitfield.expo = 255;
            b.bitfield.partFrac = 0;
            IEEHEX(b, 2);
            union Code res;
            if(num2 < 0){
                res.bitfield.sign = 1;
                ui->textBi->setText("11111111100000000000000000000000");
            }else{
                res.bitfield.sign = 0;
                ui->textBi->setText("01111111100000000000000000000000");
            }
            res.bitfield.expo = 255;
            res.bitfield.partFrac = 0;
            REALHEX(res);

        }else if(isinf(num1) && esNumero2){
            union Code b = conver.floattoIEE(num2);
            IEEHEX(b, 2);
            union Code a;
            if(num1 > 0){
                a.bitfield.sign = 0;
            }else{
                a.bitfield.sign = 1;
            }
            a.bitfield.expo = 255;
            a.bitfield.partFrac = 0;
            IEEHEX(a, 1);
            union Code res;
            if(num1 < 0){
                res.bitfield.sign = 1;
                ui->textBi->setText("11111111100000000000000000000000");
            }else{
                res.bitfield.sign = 0;
                ui->textBi->setText("01111111100000000000000000000000");
            }
            res.bitfield.expo = 255;
            res.bitfield.partFrac = 0;
            REALHEX(res);
        }else{
            cout << "Valores incorrecto" << endl;  //Val mal
        }
    }
}

void MainWindow::IEEHEX(union Code num, int pos){
    bitset<1> sign(num.bitfield.sign);
    string a = sign.to_string();
    bitset<8> exp(num.bitfield.expo);
    string b = exp.to_string();
    bitset<23> frac(num.bitfield.partFrac);
    string c = frac.to_string();
    string fin = a+b+c;

    stringstream ss;
    ss << hexfloat << num.numero;
    QString hex = QString::fromStdString(ss.str());

    switch(pos){
        case 1:
            ui->textOp1Bi->setText(QString::fromStdString(fin));
            ui->textOp1Hexa->setText(hex);
            break;
        case 2:
            ui->textOp2Bi->setText(QString::fromStdString(fin));
            ui->textOp2Hexa->setText(hex);
            break;
    }


}

void MainWindow::REALHEX(union Code num){
    string fin = to_string(num.numero);
    ui->textReal->setText(QString::fromStdString(fin));

    stringstream ss;
    ss << hexfloat << num.numero;
    QString hex = QString::fromStdString(ss.str());
    ui->textHexa->setText(hex);

}

void MainWindow::on_pushMult_clicked() //mult
{
    QString val1 = ui->textOp1Real->toPlainText();
    QString val2 = ui->textOp2Real->toPlainText();

    bool esNumero = false;
    bool esNumero2 = false;
    float num1 = val1.toFloat(&esNumero);
    float num2 = val2.toFloat(&esNumero2);
    if((num1 == 0 && isinf(num2)) || (num2 == 0 && isinf(num1))){
        if(num1 == 0 && isinf(num2)){
            union Code a = conver.floattoIEE(num1);
            IEEHEX(a, 1);
            union Code b;
            if(num2 > 0){
                b.bitfield.sign = 0;
            }else{
                b.bitfield.sign = 1;
            }
            b.bitfield.expo = 255;
            b.bitfield.partFrac = 0;
            IEEHEX(b, 2);
        }else if (num2 == 0 && isinf(num1)){
            union Code a = conver.floattoIEE(num2);
            IEEHEX(a, 2);
            union Code b;
            if(num1 > 0){
                b.bitfield.sign = 0;
            }else{
                b.bitfield.sign = 1;
            }
            b.bitfield.expo = 255;
            b.bitfield.partFrac = 0;
            IEEHEX(b, 1);
        }
        ui->textBi->setText("01111111100000000000000000000001");
        union Code a;
        a.numero = nanf("");
        REALHEX(a);
    }else{
        if (esNumero && esNumero2) {
            union Code a = conver.floattoIEE(num1);
            union Code b = conver.floattoIEE(num2);
            IEEHEX(a, 1);
            IEEHEX(b, 2);

            union Code result = alu.multiplicacion(a, b);
            if(num1 == num2 && num1 == 0){
                result.numero = 0;
            }
            bitset<1> sign(result.bitfield.sign);
            bitset<8> exp(result.bitfield.expo);
            bitset<23> frac(result.bitfield.partFrac);

            bitset<32> resultado((sign.to_ulong() << 31) | (exp.to_ulong() << 23) | frac.to_ulong());

            string re = resultado.to_string();
            ui->textBi->setText(QString::fromStdString(re));

            REALHEX(result);

        }else{
            if(isinf(num1) && isinf(num2)){
                union Code a;
                if(num1 > 0){
                    a.bitfield.sign = 0;
                }else{
                    a.bitfield.sign = 1;
                }
                a.bitfield.expo = 255;
                a.bitfield.partFrac = 0;
                IEEHEX(a, 1);
                union Code b;
                if(num2 > 0){
                    b.bitfield.sign = 0;
                }else{
                    b.bitfield.sign = 1;
                }
                b.bitfield.expo = 255;
                b.bitfield.partFrac = 0;
                IEEHEX(b, 2);
                union Code res;
                if(a.bitfield.sign != b.bitfield.sign){
                    res.bitfield.sign = 1;
                    ui->textBi->setText("11111111100000000000000000000000");
                }else{
                    res.bitfield.sign = 0;
                    ui->textBi->setText("01111111100000000000000000000000");
                }
                res.bitfield.expo = 255;
                res.bitfield.partFrac = 0;
                REALHEX(res);
            }else if(isinf(num1) && esNumero2){
                union Code b = conver.floattoIEE(num2);
                IEEHEX(b, 2);
                union Code a;
                if(num1 > 0){
                    a.bitfield.sign = 0;
                }else{
                    a.bitfield.sign = 1;
                }
                a.bitfield.expo = 255;
                a.bitfield.partFrac = 0;
                IEEHEX(a, 1);
                union Code res;
                if((num1 < 0 && num2 > 0) || (num1 > 0 && num2 < 0)){
                    res.bitfield.sign = 1;
                    ui->textBi->setText("11111111100000000000000000000000");
                }else{
                    res.bitfield.sign = 0;
                    ui->textBi->setText("01111111100000000000000000000000");
                }
                res.bitfield.expo = 255;
                res.bitfield.partFrac = 0;
                REALHEX(res);
            }else if(isinf(num2) && esNumero){
                union Code a = conver.floattoIEE(num1);
                IEEHEX(a, 1);
                union Code b;
                if(num2 > 0){
                    b.bitfield.sign = 0;
                }else{
                    b.bitfield.sign = 1;
                }
                b.bitfield.expo = 255;
                b.bitfield.partFrac = 0;
                IEEHEX(b, 2);
                union Code res;
                if((num1 < 0 && num2 > 0) || (num1 > 0 && num2 < 0)){
                    res.bitfield.sign = 1;
                    ui->textBi->setText("11111111100000000000000000000000");
                }else{
                    res.bitfield.sign = 0;
                    ui->textBi->setText("01111111100000000000000000000000");
                }
                res.bitfield.expo = 255;
                res.bitfield.partFrac = 0;
                REALHEX(res);
            }else{
                cout << "Valores incorrecto" << endl;  //Val mal
            }
        }

    }
}

void MainWindow::on_pushDiv_clicked()
{

    QString val1 = ui->textOp1Real->toPlainText();
    QString val2 = ui->textOp2Real->toPlainText();

    bool esNumero = false;
    bool esNumero2 = false;
    float num1 = val1.toFloat(&esNumero);
    float num2 = val2.toFloat(&esNumero2);

    if (esNumero && esNumero2) {
        union Code a = conver.floattoIEE(num1);
        union Code b = conver.floattoIEE(num2);
        IEEHEX(a, 1);
        IEEHEX(b, 2);

        union Code result = alu.division(a, b);

        bitset<1> sign(result.bitfield.sign);
        bitset<8> exp(result.bitfield.expo);
        bitset<23> frac(result.bitfield.partFrac);

        bitset<32> resultado((sign.to_ulong() << 31) | (exp.to_ulong() << 23) | frac.to_ulong());

        string re = resultado.to_string();
        ui->textBi->setText(QString::fromStdString(re));

        REALHEX(result);

    }else{
        if(false){

        }else{
            cout << "Valores incorrecto" << endl;  //Val mal
        }
    }

}
