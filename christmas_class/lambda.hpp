#ifndef LAMBDA_HPP
#define LAMBDA_HPP

class fibonacci
{
    int n0;
    int n1;

public:

    fibonacci() : n0(0), n1(1)
    {}

    int operator()() {
        int tmp = n0 + n1;
        n0 = n1;
        n1 = tmp;
        
        return n0;
    }
};

#endif // LAMBDA_HPP
