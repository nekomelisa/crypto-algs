#include <iostream>


int power(int x, unsigned int y, unsigned int M)
{
    if (y == 0)
        return 1;

    int p = power(x, y / 2, M) % M;
    p = (p * p) % M;

    return (y % 2 == 0) ? p : (x * p) % M;
}

int modInverse(int A, int M)
{
    int res = power (A, M-2, M );
    return res;
}

