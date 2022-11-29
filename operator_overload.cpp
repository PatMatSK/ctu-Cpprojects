#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument
{
public:
    InvalidDateException ( )
            : invalid_argument ( "invalid date or format" )
    {
    }
};
//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base & ( * date_format ( const char * fmt ) ) ( ios_base & x )
{
    return [] ( ios_base & ios ) -> ios_base & { return ios; };
}
//=================================================================================================
class CDate
{
private:
    int year;
    int month;
    int day;
    static inline int validDays [12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
public:
    static bool validityCheck( int y, int m, int d ) ;
    explicit CDate( int y, int m, int d ){
        if ( ! validityCheck(y,m,d) )
            throw new InvalidDateException;
        year = y;
        month = m;
        day = d;
    }
    CDate &     operator ++ ( );
    const CDate operator ++ ( int );
    CDate &     operator -- ( );
    const CDate operator -- ( int );
    CDate       operator + ( int x ) const;
    CDate       operator - ( int x ) const;
    int  operator - ( const CDate& x ) const;
    bool operator == ( const CDate &x ) const;
    bool operator != ( const CDate &x ) const;
    bool operator < ( const CDate &x ) const;
    bool operator > ( const CDate &x ) const;
    bool operator <= ( const CDate &x ) const;
    bool operator >= ( const CDate &x ) const;
    friend istream & operator >> ( istream & is,       CDate & x );
    friend ostream & operator << ( ostream & os, const CDate & x );
};
//---------------------------------------------------------------------------------
ostream &operator<<(ostream &os, const CDate &x) {
    return os << x.year << "-" << setfill('0') << setw(2) << x.month << "-" << setfill('0') << setw(2) << x.day;
}
//---------------------------------------------------------------------------------
istream &operator>>(istream &is, CDate &x) {
    int a,b,c;
    char d,e;
    if ( !( is >> a >> d >> b >> e >> c)
        || d != '-'
        || e != '-'
        || !CDate::validityCheck(a,b,c) ){
        is .setstate( ios::failbit );
        return is;
    }
    x.year = a;
    x.month = b;
    x.day = c;
    return is;
}
//---------------------------------------------------------------------------------
bool CDate::validityCheck(const int y, const int m, const int d) {
    if ( m < 1 || d < 1 || m > 12 )
        return false;
    if ( y % 4 == 0 && m == 2 )
        return d <= 29 ;
    else if ( d > validDays[m-1] )
        return false;
    return true;
}
//---------------------------------------------------------------------------------
CDate &CDate::operator--( ) {
    if ( day > 1 )
        day--;
    else if ( month > 1 ){
        month --;
        if ( year % 4 == 0 && month == 2 )
            day = 29;
        else
            day = validDays[month-1];
    }else{
        year--;
        month = 12;
        day = 31;
    }
    return *this;
}
//---------------------------------------------------------------------------------
CDate &CDate::operator++(  ) {
    if ( day < validDays[month-1] || ( year % 4 == 0 && month == 2 && day == 28 ) )
        day++;
    else{
        if ( month ==  12 ){
            month = 1;
            year++;
        }
        else
            month++;
        day = 1;
    }
    return *this;
}
//---------------------------------------------------------------------------------
CDate CDate::operator+( int x ) const {
    if ( x < 0 )
        return *this-(-x);
    CDate n(year,month,day);
    for ( int i = 0; i < x ; i++ )
        ++n;
    return n;
}
//---------------------------------------------------------------------------------
CDate CDate::operator-(int x) const {
    if ( x < 0 )
        return *this+(-x);
    CDate n(year,month,day);
    for ( int i = 0; i < x ; i++ )
        --n;
    return n;
}
//---------------------------------------------------------------------------------
const CDate CDate::operator++(int) {
    CDate tmp = *this;
    ++*this;
    return tmp;
}
//---------------------------------------------------------------------------------
const CDate CDate::operator--(int) {
    CDate tmp = *this;
    --*this;
    return tmp;
}
//---------------------------------------------------------------------------------
int CDate::operator-( const CDate &x) const {
    CDate tmp = *this;
    CDate tmp2 = x;
    int diff = 0;
    while ( tmp > tmp2 ){
        --tmp;
        diff++;
    }
    while ( tmp < tmp2 ){
        --tmp2;
        diff++;
    }
    return diff;
}
//---------------------------------------------------------------------------------
bool CDate::operator==(const CDate &x) const {
    return year == x.year && month == x.month && day == x.day ;
}
//---------------------------------------------------------------------------------
bool CDate::operator!=(const CDate &x) const {
    return year != x.year || month != x.month || day != x.day ;
}
//---------------------------------------------------------------------------------
bool CDate::operator<(const CDate &x) const {
    if ( year < x.year )
        return true;
    if ( year > x.year )
        return false;
    if ( month < x.month )
        return true;
    if ( month > x.month )
        return false;
    if ( day < x.day )
        return true;
    if ( day > x.day )
        return false;
    return false;
}
//---------------------------------------------------------------------------------
bool CDate::operator>(const CDate &x) const {
    if ( year > x.year )
        return true;
    if ( year < x.year )
        return false;
    if ( month > x.month )
        return true;
    if ( month < x.month )
        return false;
    if ( day > x.day )
        return true;
    if ( day < x.day )
        return false;
    return false;
}
//---------------------------------------------------------------------------------
bool CDate::operator<=(const CDate &x) const {
    return *this < x || *this == x;
}
//---------------------------------------------------------------------------------
bool CDate::operator>=(const CDate &x) const {
    return *this > x || *this == x;
}
//---------------------------------------------------------------------------------


#ifndef __PROGTEST__
int main ( void )
{

    ostringstream oss;
    istringstream iss;

    CDate a ( 2000, 1, 2 );
    CDate b ( 2010, 2, 3 );
    CDate c ( 2004, 2, 10 );
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2000-01-02" );
    oss . str ("");
    oss << b;
    assert ( oss . str () == "2010-02-03" );
    oss . str ("");
    oss << c;
    assert ( oss . str () == "2004-02-10" );
    a = a + 1500;
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2004-02-10" );
    b = b - 2000;
    oss . str ("");
    oss << b;
    assert ( oss . str () == "2004-08-13" );
    assert ( b - a == 185 );
    assert ( ( b == a ) == false );
    assert ( ( b != a ) == true );
    assert ( ( b <= a ) == false );
    assert ( ( b < a ) == false );
    assert ( ( b >= a ) == true );
    assert ( ( b > a ) == true );
    assert ( ( c == a ) == true );
    assert ( ( c != a ) == false );
    assert ( ( c <= a ) == true );
    assert ( ( c < a ) == false );
    assert ( ( c >= a ) == true );
    assert ( ( c > a ) == false );
    a = ++c;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-11 2004-02-11" );
    a = --c;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-10 2004-02-10" );
    a = c++;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-10 2004-02-11" );
    a = c--;
    oss . str ( "" );
    oss << a << " " << c;
    assert ( oss . str () == "2004-02-11 2004-02-10" );
    iss . clear ();
    iss . str ( "2015-09-03" );
    assert ( ( iss >> a ) );
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2015-09-03" );
    a = a + 70;
    oss . str ("");
    oss << a;
    assert ( oss . str () == "2015-11-12" );

    CDate d ( 2000, 1, 1 );
    try
    {
        CDate e ( 2000, 32, 1 );
        assert ( "No exception thrown!" == nullptr );
    }
    catch ( ... )
    {
    }
    iss . clear ();
    iss . str ( "2000-12-33" );
    assert ( ! ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-01-01" );
    iss . clear ();
    iss . str ( "2000-11-31" );
    assert ( ! ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-01-01" );
    iss . clear ();
    iss . str ( "2000-02-29" );
    assert ( ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-02-29" );
    iss . clear ();
    iss . str ( "2001-02-29" );
    assert ( ! ( iss >> d ) );
    oss . str ("");
    oss << d;
    assert ( oss . str () == "2000-02-29" );
/*
    //-----------------------------------------------------------------------------
    // bonus test examples
    //-----------------------------------------------------------------------------
    CDate f ( 2000, 5, 12 );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2000-05-12" );
    oss . str ("");
    oss << date_format ( "%Y/%m/%d" ) << f;
    assert ( oss . str () == "2000/05/12" );
    oss . str ("");
    oss << date_format ( "%d.%m.%Y" ) << f;
    assert ( oss . str () == "12.05.2000" );
    oss . str ("");
    oss << date_format ( "%m/%d/%Y" ) << f;
    assert ( oss . str () == "05/12/2000" );
    oss . str ("");
    oss << date_format ( "%Y%m%d" ) << f;
    assert ( oss . str () == "20000512" );
    oss . str ("");
    oss << date_format ( "hello kitty" ) << f;
    assert ( oss . str () == "hello kitty" );
    oss . str ("");
    oss << date_format ( "%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%" ) << f;
    assert ( oss . str () == "121212121212050505200020002000%%%%%" );
    oss . str ("");
    oss << date_format ( "%Y-%m-%d" ) << f;
    assert ( oss . str () == "2000-05-12" );
    iss . clear ();
    iss . str ( "2001-01-1" );
    assert ( ! ( iss >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2000-05-12" );
    iss . clear ();
    iss . str ( "2001-1-01" );
    assert ( ! ( iss >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2000-05-12" );
    iss . clear ();
    iss . str ( "2001-001-01" );
    assert ( ! ( iss >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2000-05-12" );
    iss . clear ();
    iss . str ( "2001-01-02" );
    assert ( ( iss >> date_format ( "%Y-%m-%d" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2001-01-02" );
    iss . clear ();
    iss . str ( "05.06.2003" );
    assert ( ( iss >> date_format ( "%d.%m.%Y" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2003-06-05" );
    iss . clear ();
    iss . str ( "07/08/2004" );
    assert ( ( iss >> date_format ( "%m/%d/%Y" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2004-07-08" );
    iss . clear ();
    iss . str ( "2002*03*04" );
    assert ( ( iss >> date_format ( "%Y*%m*%d" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2002-03-04" );
    iss . clear ();
    iss . str ( "C++09format10PA22006rulez" );
    assert ( ( iss >> date_format ( "C++%mformat%dPA2%Yrulez" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2006-09-10" );
    iss . clear ();
    iss . str ( "%12%13%2010%" );
    assert ( ( iss >> date_format ( "%%%m%%%d%%%Y%%" ) >> f ) );
    oss . str ("");
    oss << f;
    assert ( oss . str () == "2010-12-13" );

    CDate g ( 2000, 6, 8 );
    iss . clear ();
    iss . str ( "2001-11-33" );
    assert ( ! ( iss >> date_format ( "%Y-%m-%d" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "29.02.2003" );
    assert ( ! ( iss >> date_format ( "%d.%m.%Y" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "14/02/2004" );
    assert ( ! ( iss >> date_format ( "%m/%d/%Y" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "2002-03" );
    assert ( ! ( iss >> date_format ( "%Y-%m" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "hello kitty" );
    assert ( ! ( iss >> date_format ( "hello kitty" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "2005-07-12-07" );
    assert ( ! ( iss >> date_format ( "%Y-%m-%d-%m" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-06-08" );
    iss . clear ();
    iss . str ( "20000101" );
    assert ( ( iss >> date_format ( "%Y%m%d" ) >> g ) );
    oss . str ("");
    oss << g;
    assert ( oss . str () == "2000-01-01" );
*/
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
