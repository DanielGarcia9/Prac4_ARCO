#ifndef ALU_H
#define ALU_H


class ALU
{
public:
    ALU();
    union Code suma(union Code a, union Code b);
    union Code restar(union Code a, union Code b);
    union Code multiplicacion(union Code a, union Code b);
    union Code division(union Code a, union Code b);
    union Code negate(union Code a);
    union Code positive(union Code a);
};

#endif // ALU_H
