#include<QtCrypto>

QCA::BigInteger mult (QCA::BigInteger x, QCA::BigInteger y);

QCA::BigInteger mod_inverse(QCA::BigInteger e, QCA::BigInteger m) //somehow finding modular inverse
{
    QCA::BigInteger m0 = m, t, q;
    QCA::BigInteger x0("1"), x1("0");

    if (m == QCA::BigInteger("1"))
        return QCA::BigInteger("0");

    while (e > QCA::BigInteger("1"))
    {
        q = e;
        q /= m;

        t = m;
        m = e;
        m %= t;
        e = t;

        t = x1;
        QCA::BigInteger temp = q;
        temp *= x1;
        x1 = x0;
        x1 -= temp;
        x0 = t;
    }

    if (x0 < QCA::BigInteger("0"))
        x0 += m0;

    return x0;
}



i
