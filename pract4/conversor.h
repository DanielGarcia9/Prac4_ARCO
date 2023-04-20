#ifndef CONVERSOR_H
#define CONVERSOR_H


class Conversor
{
public:
    Conversor();
    union Code floattoIEE (float num);
    void floattoIEE2 ();
    void IEEtofloat ();
    void IEEtofloat2 ();
};

#endif // CONVERSOR_H
