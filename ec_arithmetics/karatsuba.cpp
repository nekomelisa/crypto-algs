#include <iostream>
#include <sstream>
#include <QtCrypto>


std::string add_0 (std::string a, int n)
{
    for ( int i = 0 ; i < n ; i++ )
    {
        a = "0" + a ;
    }
    return a ;
}

std::string addback_0 ( std::string a, int n)
{
    for ( int i = 0; i<n; i++)
    {
        a = a + "0";
    }
    return a;
}


int to_int ( std::string x )
{
    return ((int)x[0] - '0') ;
}

std::string remove_0 (std::string a)
{
    while (to_int(a)==0)
    {
        a.erase(0,1);
    }
    return a;
}


std::string int_to_str ( int a)
{
    std::stringstream str;
    str << a;
    std::string sa;
    str >> sa;
    return sa;
}

std::string add_str ( std::string x , std::string y )
{
    std::string result ;

    if(x.length() > y.length())
    {
        y = add_0(y, x.length() - y.length());
    }
    else
    {
        x = add_0(x, y.length() - x.length());
    }
    x = add_0 (x , 1) ;
    y = add_0 (y , 1) ;
    reverse(x.begin(), x.end());
    reverse(y.begin(), y.end());
    int m = x.length() ;
    bool ten = false ;
    for ( int i = 0 ; i < m ; i ++)
    {
        int el ;
        el = to_int (x) +  to_int (y) + ten ;
        if ( el >9 )
        {
            el = el - 10;
            ten = true ;
        }
        else
            ten = false ;
        result = int_to_str(el) + result ;
        x.erase(0, 1);
        y.erase(0, 1);
    }

    return remove_0(result);
}

std::string multi_str ( std::string x , int n )
{
    std::string result ;
    x = add_0 (x , 1) ;
    reverse(x.begin(), x.end());
    int m = x.length() ;
    int ten = 0 ;
    for ( int i = 0 ; i < m ; i ++)
    {
        int el ;
        int product ;
        product = to_int (x)*n + ten ;
        if ( product >9 )
        {
            el = product%10;
            ten = product/10;
        }
        else
        {
            ten = 0 ;
            el = product ;
        }
        result = int_to_str(el) + result ;
        x.erase(0, 1);
    }
    return result;

}

std::string karatsuba ( std::string x , std::string y )
{

    if ( x.length() == 1 && y.length() == 1)
    {
        return int_to_str (to_int(x) * to_int(y));
    }

    if(x.length() > y.length())
    {
        y = add_0(y, x.length() - y.length());
    }
    else
    {
        x = add_0(x, y.length() - x.length());
    }

    if ( x.length() %2 != 0)
    {
        x = "0" + x ;
        y = "0" + y ;
    }

    int n = x.length() ;

    std::string x1, x2, y1, y2, z ;

    for ( int i = 0 ; i < n/2 ; i ++ )
    {
        x1 = x1 + x[i] ;
        x2 = x2 + x[n/2+i];
        y1 = y1 + y[i] ;
        y2 = y2 + y[n/2+i];
    }
    std::string p1 = karatsuba ( x1 , y1) ;
    std::string p2 = remove_0 (add_str(karatsuba ( x1 , y2), karatsuba ( y1 , x2 ))) ;
    std::string p3 = karatsuba ( x2, y2 ) ;

    std::string p10 = addback_0(p1, n);
    std::string p20 = addback_0(p2, n/2);
    std::string p23 = add_str(p3, p20);
    return add_str(p10, p23);
}

QCA::BigInteger mult (QCA::BigInteger x, QCA::BigInteger y)
{
    QCA::BigInteger res;
    res.fromString( QString::fromStdString( karatsuba(x.toString().toStdString(), y.toString().toStdString())) );
    return res;
}
