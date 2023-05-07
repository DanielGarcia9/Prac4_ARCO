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

float Conversor::IEEtofloat (union Code a){
    return a.numero;
}

