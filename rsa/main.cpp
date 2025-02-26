#include <QCoreApplication>
#include <QtCrypto>

#include <QDebug>
#include <cstdio>

#include <random>
#include <qbitarray.h>
#include <QString>

#include "karatsuba.h"

#ifdef QT_STATICPLUGIN
#include "import_plugins.h"
#endif


QCA::BigInteger gcd (QCA::BigInteger a, QCA::BigInteger b) //greatst common divisor
{
    QCA::BigInteger r;
    QCA::BigInteger zero("0");
    while ((a != zero) && (b != zero))
    {
        if (a == b)
        {
            return a;
        }

        if (a > b)
        {
            a %= b;
        }
        else
        {
            b %= a;
        }
        if ( a == zero)
        {
            return b;
        }
        if (b == zero)

        {
            return a;
        }
    }

}

QCA::BigInteger lcm (QCA::BigInteger &p, QCA::BigInteger &q) //least common multiple
{
    QCA::BigInteger one("1");
    p -= one;
    q -= one;
    QCA::BigInteger res;
    res.fromString( QString::fromStdString( karatsuba(p.toString().toStdString(), q.toString().toStdString())) );
    res /= gcd(p, q);
    return res;
}

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

QBitArray BI_to_BA(QCA::BigInteger a) //BigInteger to BitArray
{
    QCA::SecureArray bb = a.toArray();
    QByteArray c = bb.toByteArray();
    QBitArray bits(c.count() * 8);

    for (int i = 0; i < c.count(); ++i)
    {
        for (int b = 0; b < 8; ++b)
        {
            bits.setBit(i * 8 + b, c.at(i) & (1 << (7 - b)));
        }
    }

    int firstOneIndex = -1;
    for (int i = 0; i < bits.size(); ++i)
    {
        if (bits.testBit(i))
        {
            firstOneIndex = i;
            break;
        }
    }

    if (firstOneIndex == -1)
    {
        return QBitArray(1, false);
    }

    QBitArray trimmedBits(bits.size() - firstOneIndex);
    for (int i = firstOneIndex, j = 0; i < bits.size(); ++i, ++j)
    {
        trimmedBits.setBit(j, bits.testBit(i));
    }

    return trimmedBits;
}

QCA::BigInteger exp_mod ( QCA::BigInteger base, QBitArray exp, QCA::BigInteger mod) //modular exponation, !exponent is BitArray!
{
    QCA::BigInteger result ("1") ;
    QCA::BigInteger x = base;
    for (int i = exp.size() - 1; i > - 1; i--)
    {

        if (exp[i] == 1 )
        {
            result.fromString(QString::fromStdString( karatsuba(result.toString().toStdString(), x.toString().toStdString())));
            result %= mod;
            x.fromString( QString::fromStdString( karatsuba(x.toString().toStdString(), x.toString().toStdString())) );
            x %= mod;

        }

        else
        {
            x.fromString( QString::fromStdString( karatsuba(x.toString().toStdString(), x.toString().toStdString())) );
            x %= mod;
        }

    }

    return result;
}

QCA::BigInteger encryption ( QCA::BigInteger m, QBitArray e, QCA::BigInteger n )
{
    QCA::BigInteger ciphertext;
    ciphertext = exp_mod(m, e, n);
    return ciphertext;
}

QCA::BigInteger decryption ( QCA::BigInteger c, QBitArray e, QCA::BigInteger n )
{
    QCA::BigInteger text;
    text = exp_mod(c, e, n);
    return text;
}

int main(int argc, char *argv[])
{
    QCA::Initializer init;
    QCoreApplication app(argc, argv);


    QCA::BigInteger p ("7358460755497088964178378457749325701846711008411434101482061762241479353219380575018054559627126633279099349124188956467791829909197756888076078854727439");
    QCA::BigInteger q ("11077686407659939816321065479055461751940325961127585042176236945406097997937584289895226411639778132615549405013258519195421433045391940657314258201734043");
    QCA::BigInteger n;
    n.fromString( QString::fromStdString( karatsuba(p.toString().toStdString(), q.toString().toStdString())) );
    QCA::BigInteger phi_n = lcm(p, q);

    qDebug().nospace() << " phi(n) " << qPrintable(phi_n.toString());


    QCA::BigInteger e ("65537");
    QCA::BigInteger d = mod_inverse(e, phi_n);

    qDebug().nospace() << " d " << qPrintable(d.toString());

    QBitArray pub_key = BI_to_BA(e);
    QBitArray priv_key = BI_to_BA(d);

    QCA::BigInteger message ("15");


    QCA::BigInteger ct = encryption( message, pub_key, n);
    qDebug().nospace() << "ct " << qPrintable(ct.toString());

    QCA::BigInteger text = decryption( ct, priv_key, n);
    qDebug().nospace() <<  "text " << qPrintable(text.toString());


    return 0;
}
