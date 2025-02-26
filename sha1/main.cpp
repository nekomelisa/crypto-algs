#include <QCoreApplication>
#include <QDebug>
#include <QByteArray>
#include <QVector>
#include<QString>

void padding(const QByteArray &message, QVector<QByteArray> &parts)
{
    QByteArray pd_message;
    quint64 size = message.size() * 8;


    pd_message.append(message);

    pd_message.append(char(0x80));
    int paddingSize = (448 - ((pd_message.size() * 8) % 512)) % 512;
    pd_message.append(QByteArray(paddingSize / 8, char(0x00)));

    for (int i = 7; i >= 0; --i)
    {
        pd_message.append(static_cast<char>((size >> (i * 8)) & 0xFF));
    }

    for (int i = 0; i < pd_message.size(); i += 64)
    {
        parts.append(pd_message.mid(i, 64));
    }
}


quint32 toUIntBigEndian(const QByteArray &data)
{
    quint32 value = 0;
    for (int i = 0; i < data.size(); ++i) {
        value = (value << 8) | static_cast<quint8>(data[i]);
    }
    return value;
}


quint32 leftRotate(quint32 value, quint8 count)
{
    return (value << count) | (value >> (32 - count));
}

quint32 func_1 (quint32 x, quint32 y, quint32 z)
{
    return (x & y) | ((~x) & z);
}

quint32 func_2 (quint32 x, quint32 y, quint32 z)
{
    return x ^ y ^ z;
}

quint32 func_3 (quint32 x, quint32 y, quint32 z)
{
    return (x & y) | (x & z) | (y & z);
}



QString sha (QByteArray message)
{
    quint32 A;
    quint32 B;
    quint32 C;
    quint32 D;
    quint32 E;

    QList<quint32> k;
    k.append(0x5a827999);
    k.append(0x6ed9eba1);
    k.append(0x8f1bbcdc);
    k.append(0xca62c1d6);

    quint32 (*function[4])(quint32 x, quint32 y, quint32 z);
    function[0] = func_1;
    function[1] = func_2;
    function[2] = func_3;
    function[3] = func_2;

    QVector<QByteArray> parts;

    padding(message, parts);


    for (int i = 0; i < parts.size(); ++i)
    {

        A = 0x67452301;
        B = 0xefcdab89;
        C = 0x98badcfe;
        D = 0x10325476;
        E = 0xc3d2e1f0;

        quint32 a = A;
        quint32 b = B;
        quint32 c = C;
        quint32 d = D;
        quint32 e = E;

        int pos = 0, arrsize = parts[i].size(), sizeInArray = 4;
        QList<QByteArray> arrays;
        while(pos<arrsize)
        {
            QByteArray arr = parts[i].mid(pos, sizeInArray);
            arrays << arr;
            pos+=arr.size();
            //ok
        }


        QVector<quint32> w(80);

        for (int t = 0; t < 80; t++)
        {
            if (t < 16)
            {
                w[t] = toUIntBigEndian(parts[i].mid(t * 4, 4));
            }
            else
            {
                w[t] = leftRotate(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1) & 0xFFFFFFFF;
            }
        }





        for (int t = 0; t < 80; t++)
        {
            quint32 temp;
            if (t < 20)
            {
                temp = (leftRotate(a, 5) + (*function[0])(b, c, d) + e +w[t] + k[0]) & 0xFFFFFFFF;
            }
            if ( t > 19 && t < 40)
            {
                temp = (leftRotate(a, 5) + (*function[1])(b, c, d) + e +w[t] + k[1]) & 0xFFFFFFFF;
            }
            if ( t > 39 && t < 60)
            {
                temp = (leftRotate(a, 5) + (*function[2])(b, c, d) + e +w[t] + k[2]) & 0xFFFFFFFF;
            }
            if ( t > 59)
            {
                temp = (leftRotate(a, 5) + (*function[3])(b, c, d) + e +w[t] + k[3]) & 0xFFFFFFFF;
            }

            e = d;
            d = c;
            c = leftRotate(b, 30);
            b = a;
            a = temp;

        }
        A = (A + a) & 0xFFFFFFFF;
        B = (B + b) & 0xFFFFFFFF;
        C = (C + c) & 0xFFFFFFFF;
        D = (D + d) & 0xFFFFFFFF;
        E = (E + e) & 0xFFFFFFFF;

    }
    QString hash;
    hash += QString("%1").arg(A, 8, 16, QChar('0'));
    hash += QString("%1").arg(B, 8, 16, QChar('0'));
    hash += QString("%1").arg(C, 8, 16, QChar('0'));
    hash += QString("%1").arg(D, 8, 16, QChar('0'));
    hash += QString("%1").arg(E, 8, 16, QChar('0'));
    return hash.toLower();


    return hash;


}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QByteArray message("1");

    QString hash = sha(message);

    qDebug().nospace() <<  qPrintable(hash);

    return 0;
}
