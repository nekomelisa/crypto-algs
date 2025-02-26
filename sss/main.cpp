#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QPair>
#include <QtGlobal>
#include <QRandomGenerator>
#include <QTime>
#include "mod_reverse.h"

int poly_inter (const QList<QPair<int, int>> &m_points,  const int &mod, const int &k)
{

    QList<int> l;
    int M = 0;

    for (int j = 0; j < k; j++)
    {
        l.append(1);
        for (int m = 0; m < k; m++)
        {

            if (m != j )
            {
                int num = mod - m_points[m].first;
                int denom = m_points[j].first - m_points[m].first;
                if (denom < 0)
                {
                    denom += mod;
                }
                denom = modInverse(denom, mod);
                l[j] = (l[j] *num * denom) % mod;

            }
        }

        M = ( M + l[j] * m_points[j].second) % mod;
    }
    return M;

}

int func_x (QList<int> &func, int & x, int &mod)
{
    int y = 0;
    int n = func.length();
    int delta = 1;
    for (int i = 0; i < n; i++)
    {
        y = (y + func[n-i-1] * delta) % mod;
        delta = delta * x % mod;
    }
    return y;
}

QList<QPair<int, int>> rand_points(const QList<QPair<int, int>> &pairList, int m)
{
    QList<QPair<int, int>> shuffledList = pairList;

    for (int i = 0; i < shuffledList.size(); ++i)
    {
        int randomIndex = qrand() % shuffledList.size();
        shuffledList.swapItemsAt(i, randomIndex);
    }


    QList<QPair<int, int>> selectedPairs;
    for (int i = 0; i < m && i < shuffledList.size(); ++i)
    {
        selectedPairs.append(shuffledList[i]);
    }

    return selectedPairs;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int M = 122;
    int mod = 547;
    int m = 10;
    int n = 500;

    QList <int> func;
    for (int i = 0; i < m-1; i++)
    {
        func.append( QRandomGenerator::global()->bounded(mod));
    }
    func.append(M);

    QList <QPair<int, int>> points, m_points;

    for (int i = 0; i < n; i++)
    {
        int x = i + 1;
        int y = func_x( func, x, mod);
        points.append(qMakePair(x, y));
    }

    m_points = rand_points(points, m);

    int message = poly_inter(m_points, mod, m);
    qDebug() << message;

    return 0;


}



