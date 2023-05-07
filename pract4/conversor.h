#ifndef CONVERSOR_H
#define CONVERSOR_H


class Conversor
{
public:
    Conversor();
    union Code floattoIEE (float num);
    float IEEtofloat (union Code a);
};

#endif // CONVERSOR_H
