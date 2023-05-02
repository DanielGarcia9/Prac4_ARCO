#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "conversor.h"
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


void MainWindow::on_pushPlus_clicked() //SUMA
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
                bitset<1> g(0), r(0), st(0);
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
                cout << "D " << d << endl;

                if(signA != signB){
                    //desplazar los ultimos

                    P >>= d;

                    for(int i = 0; i < d; i++){
                        P[23-i] = 1;
                    }
                }else{
                    P >>= d;
                }

                bitset<24> res;
                int acarreo = 0;   //Suma binaria con acarreo

                for (int i = 0; i < 24; i++) {
                    int suma = mantisaA[i] + P[i] + acarreo;
                    res[i] = suma % 2;
                    acarreo = suma / 2;
                }

                P = res;

                if(signA != signB && P[23] == 1 && acarreo == 0){
                //solo ocurre si d = 0
                    P = ~P;
                    P = P.to_ulong()+1;
                    complemP = true;
                }  //Hasta aqui bien


                if (signA == signB && acarreo == 1){
                    st = st | r | g;
                    r[0] = P[0];

                    P >>= 1;
                    P[23] = acarreo;
                    cout << expSuma << endl;
                    expSuma = expSuma.to_ulong() + 1;
                    cout << expSuma << endl;
                }else {                                             //1E-37 + 1E-37 deberia ser exponente 5
                    cout << P << endl;
                    int k = 0;//numero de bits desplazar p para que sea mantisa normalizada, matisa normalizada tiene unicamente 24 bits y empieza por 1
                    for (int i = 23; i >= 0; i--) {
                        if (P[i] == 1) {
                            k++;
                            break;
                        }
                        k++;
                    }
                    cout << "K " << k << endl;   //Algo sigue estando mal
                    if (k == 0){
                        st = r | st;
                        r = g;
                    }if (k > 1){
                        r[0] = 0;
                        st[0] = 0;
                    }
                    k--;
                    P <<= k;
                    cout << "g " << g << endl;
                    for (int i = 0; i < k; i++){
                        //desplazar p y g a la izq k bits
                        P[i] = g[0];
                    }
                    cout << P << endl;
                    cout << expSuma.to_ulong() << endl;
                    expSuma = expSuma.to_ulong() - k;
                    cout << expSuma.to_ulong() << endl;
                }

                if ((r[0] == 1 && st[0] == 1) || (r[0] == 1 && st[0] == 0 && P[0]==1)){
                    cout << "entra" << endl;
                    int acarreo2 = 0;
                    if(P[0] == 1){
                        acarreo2 = 1;
                    }
                    P = P.to_ulong() + 1;

                    if(P.to_ulong() > (1<<24) - 1){
                        acarreo2 = 1;
                    }
                    if(acarreo2 == 1){
                        P >>= 1;
                        P[23] = acarreo2;
                        expSuma = expSuma.to_ulong() +1;
                    }

                }
                cout << expSuma << endl;
                bitset<24> mantSuma = P;
                bitset<1> signSuma;
                if (opIntercambiados == false && complemP == true){
                      signSuma = signB;
                }else {
                      signSuma = signA;
                }
                cout << mantSuma << endl;
                bitset<32> resultado;
                for(int i = 0; i < 23; i++){
                    resultado[i] = mantSuma[i];
                }
                for(int i = 0; i < 8; i++){
                    resultado[23+i] = expSuma[i];
                }
                resultado[31] = signSuma[0];

                string re = resultado.to_string();
                ui->textBi->setText(QString::fromStdString(re));

                union Code resu;
                resu.bitfield.sign = signSuma.to_ulong();
                resu.bitfield.expo = expSuma.to_ulong();
                bitset<23> frac;
                for(int i = 0; i < 23; i++){
                    frac[i] = resultado[i];
                }
                resu.bitfield.partFrac = frac.to_ulong();
                REALHEX(resu);

                cout << resultado << endl;      //FUNCIONAN TODAS LAS PRUEBAS MENOS CON E40 y 2E-37
            }
        }else{
            if(isinf(num2)){
                cout << "inf" << endl; //inf
            }else{
                cout << "mal2" << endl;  //Val mal
            }

        }
    } else {
        if(isinf(num1)){
            cout << "inf" << endl; //inf
        }else{
            cout << "mal" << endl;  //Val mal
        }
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

                int n = 24;

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

                //paso 1
                bitset<1> signProd;
                if(signA[0] == signB[0]){
                    signProd[0] = 0;
                }else{
                    signProd[0] = 1;
                }
                //paso 2
                //estan en exceso? si es asi restar 127
                bitset<8> expProd;
                int aux = expA.to_ulong() + expB.to_ulong() - 127;
                if(aux > 255){
                    cout << "El numero es infinito" << endl;
                }else{
                    expProd = (expA.to_ulong()-127) + (expB.to_ulong()- 127) + 127;
                    bitset<24> P(0);
                    int c = 0;
                    for (int i = 0; i < 24; i++){
                        if (mantisaA[0] == 1){
                            c = 0;
                            for (int i = 0; i < 24; i++) {
                                int suma = P[i] + mantisaB[i] + c;
                                P[i] = suma % 2;
                                c = suma / 2;
                            }
                        }else {
                            //P = P + 0?
                        }
                        bitset<1> lastBit;
                        lastBit[0] = P[0];
                        P>>= 1;
                        P[23] = c;
                        c = 0;
                        mantisaA >>= 1;
                        mantisaA[23] = lastBit[0];

                    }

                    bitset<48> PA;

                    PA = (P.to_ulong() << 24) | mantisaA.to_ulong();



                    if (PA[n-1] == 0){
                        PA <<= 1;
                    }else {
                        expProd = expProd.to_ulong() + 1;
                        cout << expProd <<endl;
                    }


                    bitset<1> r;
                    r[0] = mantisaA[n-1];//bit redondeo

                    bitset<1> st = 0;//Bit sticky
                    for (int i = n - 2; i >= 0; i--) {
                        st[0] = st[0] | mantisaA[i];
                    }

                    if ((r[0] == 1 && st[0] == 1) || (r[0] == 1 && st[0] == 0 && P[0] == 1)){
                        P = P.to_ulong() + 1;
                    }
                    //desbordamientos
                    int expMax= 255;
                    int expMin= 1;
                    if (expProd.to_ulong() > expMax){
                        //overflow

                    }else {
                        if (expProd.to_ulong() < expMin){
                            int t = expMin - expProd.to_ulong();
                            if (t >= 24){
                                //underflow
                                cout << "Not a number, not enought bits to show the number" << endl;
                            }else {
                                //desplazar (P,A) t bits derecha
                                bitset<1> aux;
                                aux[0] = mantisaA[23];

                                PA >>= t;
                              // Colocar los bits de A en P a partir de la posición pos
                                for(int i = 0; i < t; i++){
                                    PA[23-i] = aux[0];
                                }
                                expProd = expMin;
                            }
                        }
                    }
                    //operandos denormales
                    /*if (expProd.to_ulong() > expMin){
                        int t1 = expProd.to_ulong() - expMin;
                        int t2 = 0;//numero de bits desplazar (P,A) hacia izq para que sea normalizada
                        for (int i = 23; i >= 0; i--) {
                            if (P[i] == 1) {
                                break;
                            }
                            t2++;
                        }
                        int t = min(t1,t2);
                        expProd = expProd.to_ulong() - t;
                        //desplazar (P,A) t bits izq    ARITMETICAMENTE??
                        PA <<= t;

                    }else if(expProd.to_ulong() == expMin) {
                        //resultado un denormal directamente

                    }*/
                    //tener en cuenta operandos 0
                    //0 x n = 0
                    //0 x +-infinito = indeterminacion(NaN)
                    bitset<23> mP;
                    for(int i = 0; i < 23; i++){
                        mP[22 - i] = PA[46-i];
                    }

                    cout << signProd << expProd << mP << endl;
                }
                }

                //paso 3


        }
}

void MainWindow::on_pushDiv_clicked()
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


            //Para escalar hay q normalizar?
            bitset<23> partFrac;   //ReguRegu
            for(int i = 0; i < 23; i++){
                partFrac[i] = mantisaA[i];
            }
            unsigned int decimal = partFrac.to_ulong();
            double fraccionario = decimal / std::pow(2, 23);
            float valor = 1 + fraccionario;
            std::cout << "El valor fraccionario es: " << valor << std::endl;


        }else{
            if(isinf(num2)){
                cout << "inf" << endl; //inf
            }else{
                cout << "mal2" << endl;  //Val mal
            }

        }
    } else {
        if(isinf(num1)){
            cout << "inf" << endl; //inf
        }else{
            cout << "mal" << endl;  //Val mal
        }
    }

}
