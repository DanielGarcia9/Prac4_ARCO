#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "conversor.h"
#include <iostream>
#include <bitset>
#include <sstream>

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
            IEEHEX(a, 1);
            union Code b = conver.floattoIEE(num2);
            IEEHEX(b, 2);
            bitset<1> signA(a.bitfield.sign);
            bitset<1> signB(b.bitfield.sign);
            bitset<8> expA(a.bitfield.expo);
            bitset<8> expB(b.bitfield.expo);

            int* bin = reinterpret_cast<int*>(&a.numero); //repre binaria del float
            bitset<32> bits(*bin);
            bitset<24> mantisaA;
            for(int i = 0; i < 24; i++){
                mantisaA[i] = bits[i];
            }

            int* bin2 = reinterpret_cast<int*>(&b.numero); //repre binaria del float
            bitset<32> bits2(*bin2);
            bitset<24> mantisaB;
            for(int i = 0; i < 24; i++){
                mantisaB[i] = bits2[i];
            }

            if(signA != signB && expA == expB && mantisaA == mantisaB){
                union Code res;
                res.numero = 0;
                string aux = to_string(res.numero);
                ui->textReal->setText(QString::fromStdString(aux));
                IEEHEX(res, 3);
            }else{
                bitset<1> g(0);
                bitset<1> r(0);
                bitset<1> st(0);
                bitset<24> P;
                int n = 24;
                bool opIntercambiados = false;
                bool complemP = false;

                if(expA.to_ulong() < expB.to_ulong()){
                    bitset<1> auxSig = signA;
                    signA = signB;
                    signB = auxSig;
                    bitset<8> auxExp = expA;
                    expA = expB;
                    expB = auxExp;
                    bitset<24> auxMant = mantisaA;
                    mantisaA = mantisaB;
                    mantisaB = auxMant;
                    opIntercambiados = true;
                }

                bitset<8> expSuma = expA;
                int d = expA.to_ulong() - expB.to_ulong();

                if(d < 0){
                    cout << "algo mal" <<endl;
                }

                if(signA != signB){
                    mantisaB = ~mantisaB;
                    mantisaB = mantisaB.to_ulong()+1;
                }

                P = mantisaB;

                if(d-1 >= 0){
                    g[0] = P[d-1];
                }
                if(d-2 >= 0){
                    r[0] = P[d-2];
                }

                int aux = d-3;
                if(aux >= 0){
                    st[0] = P[aux];
                    while(aux >= 0){
                        //or de todos los bits desde d-3 a 0
                        aux--;
                        st[0] = st[0] | P[aux];
                    }
                }

                if(signA != signB){
                    //desplazar los ultimos
                    for(int i = 0; i < d; i++){
                        P[23-i] = 1;
                    }
                }else{
                    for(int i = 0; i < d; i++){
                        P[23-i] = 0;
                    }
                }
                cout << mantisaA << endl;
                cout << P << endl;
                bitset<24> res;
                int acarreo = 0;   //Suma binaria con acarreo

                for (int i = 0; i < 24; i++) {
                    int suma = mantisaA[i] + P[i] + acarreo;
                    res[i] = suma % 2;
                    acarreo = suma / 2;
                }
                /*for(int i = 23; i >= 0; i--){
                    if(mantisaA[i] == 0 && P[i] == 0){  // 0 y 0
                        if(acarreo == 0){
                            res[i] = 0;
                        }else{
                            res[i] = 1;
                            acarreo = 0;
                        }
                    }else if(mantisaA[i] == 1 && P[i] == 1){  // 1 y 1
                        if(acarreo == 0){
                            res[i] = 0;
                            acarreo = 1;
                        }else{
                            res[i] = 1;
                            acarreo = 1;
                        }
                    }else{ // 1 y 0
                        if(acarreo == 0){
                            res[i] = 1;
                        }else{
                            res[i] = 0;
                            acarreo = 1;
                        }
                    }
                }*/

                P = res;

                if(signA != signB && P[n-1] == 1 && acarreo == 0){
                    //solo ocurre si d = 0
                    P = ~P;
                    complemP = true;
                }

                if (signA == signB && acarreo == 1){
                    st = st || r || g; //triplazo
                    r = P[0];

                    P[0] = acarreo;
                    for (int i = 2; i < 24; i++){
                        aux = P[i-1];
                        P[i] = aux;
                    }
                    expSuma = expSuma + 1;
                }else {
                    int k;//numero de bits desplazar p para que sea mantisa normalizada, matisa normalizada tiene unicamente 24 bits y empieza por 1
                    if (k == 0){
                        st = r || st;
                        r = g;
                    }if (k > 1){
                        r = 0;
                        st = 0;
                    }
                    for (int i = 0; i < 24; i++){
                        //desplazar p y g a la izq k bits
                    }
                    expSuma = expSuma - k;
                }

                if ((r = 1 && st = 1) || (r = 1 && st = 0 && P[0]==1)){
                    P = P+1; // terner en cuenta acarreo como en el punto 8
                    acarreo2 = 0;
                    P[0] = acarreo2;
                    for (int i = 2; i < 24; i++){
                        aux = P[i-1];
                        P[i] = aux;
                    }
                    expSuma = expSuma + 1;
                }
                bitset<24> matSuma = P;
                bitset<1> signSuma;
                if (opIntercambiados = false && complemP = true){
                      signSuma = signB;
                }else {
                      signSuma = signA;
                }

                int resultado = signSuma*matSuma // * 2^es
            }
        }else{
            cout << "mal2" << endl;  //Val mal
        }
    } else {
        cout << "mal" << endl;   //Val mal
    }
}

void MainWindow::IEEHEX(union Code num, int pos){ //1 op1    2 op2   3 res
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
        case 3:
            ui->textBi->setText(QString::fromStdString(fin));
            ui->textHexa->setText(hex);
            break;
    }


}

