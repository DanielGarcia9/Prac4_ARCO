#include "alu.h"
#include <iostream>
#include <bitset>
#include <sstream>
#include <cmath>
#include "conversor.h"

using namespace std;

union Code {

    struct{
        unsigned int partFrac : 23;
        unsigned int expo : 8;
        unsigned int sign : 1;
    }bitfield;

    float numero;
    unsigned int numerox;
};

ALU::ALU()
{
Conversor conver;
}

union Code ALU::suma(union Code a, union Code b){
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

        return res;

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


        union Code resu;
        resu.bitfield.sign = signSuma.to_ulong();
        resu.bitfield.expo = expSuma.to_ulong();
        bitset<23> frac;
        for(int i = 0; i < 23; i++){
            frac[i] = resultado[i];
        }
        resu.bitfield.partFrac = frac.to_ulong();

        return resu;
    }
}


union Code ALU::multiplicacion(union Code a, union Code b){
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
        union Code res;
        res.bitfield.sign = signProd.to_ulong();
        res.bitfield.expo = expProd.to_ulong();
        res.bitfield.partFrac = mP.to_ulong();

        cout << signProd << expProd << mP << endl;

        return res;
    }

}

union Code ALU::restar(union Code a, union Code b)
{
    b.bitfield.sign = 1;
    return this->suma (a,b);
}

union Code ALU::division(union Code a, union Code b)
{
        Conversor conver;

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

        bitset<23> partFracA;
        for(int i = 0; i < 23; i++){
            partFracA[i] = mantisaA[i];
        }
        unsigned int decimalA = partFracA.to_ulong();
        double fraccionarioA = decimalA / std::pow(2, 23);
        float valorA = 1 + fraccionarioA;
        std::cout << "El valor fraccionario de A es: " << valorA << std::endl;


        bitset<23> partFracB;
        for(int i = 0; i < 23; i++){
            partFracB[i] = mantisaA[i];
        }
        unsigned int decimalB = partFracB.to_ulong();
        double fraccionarioB = decimalB / std::pow(2, 23);
        float valorB = 1 + fraccionarioB;
        std::cout << "El valor fraccionario de B es: " << valorB << std::endl;

        float Bprima;
        if (valorB > 1 && valorB < 1.25) {
            Bprima = 1;
        }else if (valorB > 1.25 && valorB < 2){
            Bprima = 0.8;
        }

        union Code A = conver.floattoIEE(valorA);

        union Code B = conver.floattoIEE(valorB);

        union Code Bp = conver.floattoIEE(Bprima);


        union Code X = this->multiplicacion (A, Bp);

        union Code Y = this->multiplicacion (B, Bp);


        union Code R;
        union Code auxY;
        union Code auxX;
        union Code resultado;
        union Code val = conver.floattoIEE(2);

        while (1){

            R = this->restar (val,Y);
            auxY = this->multiplicacion (Y,R);
            auxX = this->multiplicacion (X,R);
            resultado = this->restar(auxX,X);
            float num = resultado.numero;

            if (num < 0.0001){
                break;
            }
            X = auxX;
            Y = auxY;
        }

        bitset<1> signDiv;
        signDiv[0] = signA[0] ^ signB[0];


        bitset<1> signo(auxX.bitfield.sign);
        bitset<8> expo(auxX.bitfield.expo);
        bitset<23> fracc(auxX.bitfield.partFrac);

        bitset<8> expDiv;

        expDiv = expA.to_ulong ()- expB.to_ulong ()+ expo.to_ulong ();

        union Code res;
        res.bitfield.sign = signDiv.to_ulong();
        res.bitfield.expo = expDiv.to_ulong();
        res.bitfield.partFrac = fracc.to_ulong();

        cout << signDiv << expDiv << fracc << endl;

        return res;

}



