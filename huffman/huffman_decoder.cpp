#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>

using namespace std;
typedef struct TNode {
    string letter;
    struct TNode * left = nullptr;
    struct TNode * right = nullptr;
}TNODE;

void dellTNode( TNODE * jano ){
    if ( jano->left )
        dellTNode(jano->left);
    if ( jano->right )
        dellTNode( jano->right );
    delete jano;
}

bool binak ( const char * inFileName, vector<char> & data ){
    ifstream jano;
    jano.open( inFileName, ios::binary | ios::in);
    if ( jano.fail() )
        return true;
    char c;
    while ( jano.get(c) )
        for (int i = 7; i >= 0; i--) // or (int i = 0; i < 8; i++)  if you want reverse bit order in bytes
            data.push_back( (char)(((c >> i) & 1)+48) );

    if ( data.empty() || !jano.eof() )
        return true;
    jano.close();
    return false;
}


string findChar( vector<char> & data, int * index , int s , bool * kon ){
    string out;
    for ( int j = 0; j < s; j++ ){
        int v = 0;
        for ( int i = 7 ; i >= 0 ; i-- )
            v += ( (data[ ++(*index) ] - 48 ) * ( 1 << i ) );
        out += (char)v;
    }
    return out;
}

string findhim( vector<char> & data, int * index, bool * kon )
{
    string res;
    if ( data[(*index)+1] == '0' ){
        res += findChar(data,index,1,kon);
    }
    else{
        if ( data[(*index)+2] != '1' ){
            *kon = false;
            return res;
        }
        if ( data[(*index)+3] == '0' ){
            if ( data[(*index)+9] != '1' || data[(*index)+10] != '0'){
                *kon = false;
                return res;
            }
            res += findChar(data,index,2,kon);
        }
        else if ( data[(*index)+4] == '0' ){
            if (  data[(*index)+9] != '1' ||
                  data[(*index)+10] != '0' ||
                  data[(*index)+17] != '1' ||
                  data[(*index)+18] != '0'){
                *kon = false;
                return res;
            }
            res += findChar(data,index,3,kon);
        }
        else if ( data[(*index)+5] == '0' ){
            if ( data[(*index)+9] != '1' ||
                 data[(*index)+10] != '0' ||
                 data[(*index)+17] != '1' ||
                 data[(*index)+18] != '0' ||
                 data[(*index)+25] != '1' ||
                 data[(*index)+26] != '0'){
                *kon = false;
                return res;
            }
            else{
                res += findChar(data,index,4,kon);
            }
        }
    }
    return res;
}

TNode * findLeaf( vector<char> & data, int * index , bool * kon )
{
    TNODE * ivo = new TNODE;
    if ( data[ *index ] == '0' )
    {
        (*index)++;
        ivo->left = findLeaf(data, index, kon);
        (*index)++;
        ivo->right = findLeaf(data, index, kon );
    }
    else
    {
        ivo->left = nullptr;
        ivo->right = nullptr;
        ivo->letter = findhim( data, index, kon );
    }
    if ( !kon )
        return nullptr;
    return ivo;
}

int giveCount( vector<char> & data, int * index , bool * flag){
    int pocet_znakov = 0;
    if ( (*index) + 1 >= ((int)data.size())-1 )
        return 0;
    if ( data[ ++(*index) ] == '1' )
        return 4096;
    for( int i  = 11 ; i >= 0 ; i-- )
        pocet_znakov += ( (data[ ++(*index) ]-48 ) * ( 1 << i ) ) ;
    (*flag) = false;
    return pocet_znakov ;
}

bool lastCheck( vector<char> &data, int & pocet_znakov, int * index ){
    int i = 0;
    int velkost = (int)data.size();
    while ( (*index) < velkost ){
        if ( data[(*index)+i] == '1' )
            return true;
        (*index)++;
    }
    return false;
}

bool display( ofstream & fero , vector<char> & res ){
    for ( char re : res )
        if ( !(fero << re ) )
            return true;
    return false;
}

bool makeText ( vector<char> & data, TNODE * root , const char * outFileName, int * index  ){
    ofstream fero;
    fero.open( outFileName,  std::ios::out | std::ios::binary );
    if ( fero.fail() )
    {
        dellTNode(root);
        return false;
    }
    TNODE ** jablon = &root;
    bool flag = true;
    int pocet_znakov = giveCount( data, index , &flag );
    while ( (*index) < (int)data.size() -1 && ( flag ||  pocet_znakov ))
    {
        if ( (*jablon)->left == nullptr && (*jablon)->right == nullptr ){
            if ( !(fero << (*jablon)->letter)  ){
                dellTNode(root);
                return false;
            }
            jablon = &root;
            pocet_znakov-- ;
            if ( pocet_znakov == 0 && flag )
                pocet_znakov = giveCount( data, index , &flag );
        }
        (*index)++;
        if ( data[(*index)] == '0' )
            jablon = &(*jablon)->left;
        if ( data[(*index)] == '1' )
            jablon = &(*jablon)->right;
    }
    if ( pocet_znakov == 1 && (*index) == (int)data.size() -1 ){
        if ((*jablon)->right == nullptr && (*jablon)->left == nullptr)
            fero << (*jablon)->letter ;
        else{
            dellTNode(root);
            fero.close();
            return false;
        }pocet_znakov--;
    }
    dellTNode(root);
    fero.close();
    if ( pocet_znakov != 0 || lastCheck(data,pocet_znakov,index) )
        return false;
    return true;
}

bool decompressFile ( const char * inFileName, const char * outFileName )
{
    int index = 0;
    vector<char> data;
    if ( binak(inFileName,data) )
        return false;

    TNODE * root = nullptr;
    bool kon = true;
    root = findLeaf( data, &index, &kon );

    if ( !kon ){
        dellTNode(root);
        return false;
    }
    return makeText( data, root , outFileName , &index );
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
    // keep this dummy implementation (no bonus) or implement the compression (bonus)
    return false;
}


bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
    ifstream jano;
    ifstream fero;
    jano.open(fileName1);
    fero.open(fileName2);
    string j_riadek;
    string f_riadek;
    while ( getline(jano,j_riadek) && getline(fero,f_riadek) )
        if ( j_riadek != f_riadek ){
            jano.close();
            fero.close();
            return false;
        }
    jano.close();
    fero.close();
    return true;
}


int main ( void )
{


    assert ( decompressFile ( "teste/test0.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/test0.orig", "tempfile" ) );


    assert ( decompressFile ( "teste/test1.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/test1.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/test2.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/test2.orig", "tempfile" ) );


    assert ( decompressFile ( "teste/test3.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/test3.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/test4.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/test4.orig", "tempfile" ) );


    assert ( ! decompressFile ( "teste/test5.huf", "tempfile" ) );

    decompressFile ( "teste/extra0.huf", "tempfile" ) ;
    assert ( identicalFiles ( "teste/extra0.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra1.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra1.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra2.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra2.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra3.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra3.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra4.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra4.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra5.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra5.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra6.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra6.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra7.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra7.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra8.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra8.orig", "tempfile" ) );

    assert ( decompressFile ( "teste/extra9.huf", "tempfile" ) );
    assert ( identicalFiles ( "teste/extra9.orig", "tempfile" ) );


    assert ( ! decompressFile ( "input.bin", "tempfile" ) );
    return 0;
}
