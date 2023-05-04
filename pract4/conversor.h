#ifndef CONVERSOR_H
#define CONVERSOR_H


class Conversor
{
public:
    Conversor();
    union Code floattoIEE (float num);
    void floattoIEE2 ();
    float IEEtofloat (union Code a);
    void IEEtofloat2 ();
};

#endif // CONVERSOR_H
