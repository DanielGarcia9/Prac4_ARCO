#include "conversor.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream>

Conversor::Conversor()
{

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


union Code Conversor::floattoIEE (float num){
    union Code a;
    a.numero = num;
    return a;


}


void Conversor::floattoIEE2 (){

    union Code a;
    a.numero=2.1;
    printf("Numero a imprimir: %f \n", a.numero);
    //printf("Numero unsigned: %u \n", a.numerox);
    printf("Signo: %u \n",a.bitfield.sign);
    printf("Exponente: %u \n",a.bitfield.expo);
    printf("Parte Fraccionaria : %u \n", a.bitfield.partFrac);

}

/*void Conversor::IEEtofloat (union Code a){

    bitset<1> b(a.bitfield.sign);
    bitset<8> c(a.bitfield.expo);
    bitset<23> d(a.bitfield.partFrac);

    a.bitfield.sign = b.to_ulong();
    a.bitfield.expo = c.to_ulong();
    a.bitfield.partFrac = d.to_ulong();

    printf("El número pasado a real es: %f \n", a.numero);
}*/

void Conversor::IEEtofloat2 (){

    union Code a;

    a.bitfield.partFrac = 4718592;
    a.bitfield.expo = 131;
    a.bitfield.sign = 0;
    printf("El número es: %f \n", a.numero);

}
