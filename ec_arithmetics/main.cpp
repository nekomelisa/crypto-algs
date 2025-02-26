#include <QCoreApplication>
#include <QtCrypto>
#include <QDebug>
#include <QBitArray>
#include <QString>

#include "mod_inverse.h"

#ifdef QT_STATICPLUGIN
#include "import_plugins.h"
#endif

QCA::BigInteger mult (QCA::BigInteger x, QCA::BigInteger y);
QBitArray BI_to_BA(QCA::BigInteger a);

struct Point
{
    QCA::BigInteger x;
    QCA::BigInteger y;
};

void print_point (Point a)
{
    qDebug().nospace() << "x: " << qPrintable(a.x.toString()) << " y: " << qPrintable(a.y.toString());
}

Point chords_method (const Point &a, const Point &b, const QCA::BigInteger &mod)
{
    //qDebug() << "chords method";
    QCA::BigInteger zero ("0");
    QCA::BigInteger y = a.y;
    y -= b.y;
    while (y < zero)
    {
        y += mod;
    }
    QCA::BigInteger x = a.x;
    x -= b.x;
    while (x < zero)
    {
        x += mod;
    }
    QCA::BigInteger x_inversed = mod_inverse(x, mod);
    QCA::BigInteger s = mult(y, x_inversed);
    s %= mod;

    Point c;
    c.x = mult (s, s);
    c.x %= mod;
    c.x -= a.x;
    while (c.x < zero)
    {
        c.x += mod;
    }
    c.x -= b.x;
    while (c.x < zero)
    {
        c.x += mod;
    }

    QCA::BigInteger ax = a.x;
    ax -= c.x;
    while (ax < zero)
    {
        ax += mod;
    }
    c.y = mult(s, ax);
    c.y %= mod;
    c.y -= a.y;
    while (c.y < zero)
    {
        c.y += mod;
    }
    return c;
}

Point doubling (const Point &a, const QCA::BigInteger &mod,  QCA::BigInteger A = QCA::BigInteger("0"))
{
    //qDebug() << " a == b, doubling method";
    QCA::BigInteger zero("0");
    Point c;

    if ( (a.x == zero) && (a.y == zero))
    {
        c.x = zero;
        c.y = zero;
        return c;
    }

    QCA::BigInteger ax = mult(QCA::BigInteger("3"), mult(a.x, a.x));
    ax += A;
    ax %= mod;
    QCA::BigInteger ay = mult(QCA::BigInteger("2"), a.y);
    ay %= mod;
    QCA::BigInteger ay_inversed = mod_inverse(ay, mod);
    QCA::BigInteger s = mult(ax, ay_inversed);
    s %= mod;

    c.x = mult(s, s);
    c.x %= mod;
    c.x -= mult (QCA::BigInteger("2"), a.x);
    while (c.x < 0)
    {
        c.x += mod;
    }
    ax = a.x;
    ax -= c.x;
    ax %= mod;
    c.y = mult(s, ax);
    c.y %= mod;
    c.y -= a.y;
    while (c.y < zero)
    {
        c.y += mod;
    }
    return c;

}

Point reversed (const Point &a, const Point &b)
{
    //qDebug() << " a == -b, reversed method";
    Point c;
    c.x = QCA::BigInteger ("0");
    c.y = QCA::BigInteger ("0");
    return c;
}

Point add_zero (const Point &a, const Point &b)
{
    //qDebug() << " a == 0 or b == 0, adding zero method";
    Point c;
    c.x = QCA::BigInteger("0");
    c.y = QCA::BigInteger("0");
    c.x += a.x;
    c.x += b.x;
    c.y += a.y;
    c.y += b.y;
    return c;
}

bool is_reversed (const Point &a, const Point &b, const QCA::BigInteger &mod)
{
    if (a.x == b.x)
    {
        QCA::BigInteger ay = a.y;
        ay += b.y;
        if (ay == mod)
       {
           return true;
       }
    }
    return false;
}

bool is_equal (const Point &a, const Point &b)
{
    if ( (a.x == b.x) && (a.y == b.y))
    {
        return true;
    }
    return false;
}

bool is_zero (const Point &a, const Point &b)
{

    QCA::BigInteger zero("0");
    if (((a.x == zero) && (a.y == zero)) || ((b.x == zero) && (b.y == zero)))
    {
        return true;
    }
    return false;
}

Point addition (const Point &a, const Point &b, const QCA::BigInteger &mod, QCA::BigInteger A = QCA::BigInteger("0"))
{
    Point c;

    if (is_zero (a, b) == 1)
    {

        c = add_zero(a ,b);
    }
    else
    {
        if (is_equal(a, b) == 1)
        {
            c = doubling(a, mod, A);
        }
        else
        {
            if (is_reversed(a, b, mod) == 1)
            {
                c = reversed(a, b);
            }
            else
            {
                c = chords_method(a, b, mod);
            }
        }
    }

    return c;
}

Point multiplication (Point a, QCA::BigInteger n, QCA::BigInteger mod, QCA::BigInteger A = QCA::BigInteger("0"))
{
    QBitArray bits = BI_to_BA(n);

    QString result;
    for (int i = 0; i < bits.size(); ++i)
    {
        result.append(bits.testBit(i) ? '1' : '0');
    }
    qDebug() << "n (decimal):" << n.toString();
    //qDebug() << "Bit representation:" << result;

    Point res;
    res.x = QCA::BigInteger("0");
    res.y = QCA::BigInteger("0");

    Point temp = a;
    for (int i = bits.size() - 1; i >= 0; i--)
    {

        if (bits[i] == 1)
        {
            res = addition(res, temp, mod, A);
        }
        temp = doubling(temp, mod, A);
    }


    return res;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCA::Initializer init;

    QCA::BigInteger p("115792089237316195423570985008687907853269984665640564039457584007908834671663");

    Point g;
    g.x = QCA::BigInteger("55066263022277343669578718895168534326250603453777594175500187360389116729240");
    g.y = QCA::BigInteger("32670510020758816978083085130507043184471273380659243275938904335757337482424");

    QCA::BigInteger n("42656861080476692735712490451463500659964282088439556770099615795319709895862");

    print_point( multiplication(g, n, p));



    return 0;
}
