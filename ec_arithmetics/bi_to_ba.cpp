#include <QtCrypto>
#include <QBitArray>

QBitArray BI_to_BA(QCA::BigInteger a)
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

