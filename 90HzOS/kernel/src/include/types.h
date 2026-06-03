#ifndef TYPES_H
    #define TYPES_H

    enum _null {
        _NULL_ = -1,
    };

    enum boolean {
        True  = 1,
        False = 0,
    };

    typedef unsigned    int     u32;
    typedef unsigned    int*    pu32;
    typedef signed      int     i32;
    typedef signed      int*    pi32;
    typedef unsigned    short   u16;
    typedef unsigned    short*  pu16;
    typedef signed      short   i16;
    typedef signed      short*  pi16;
    typedef unsigned    char    u8;
    typedef unsigned    char*   pu8;
    typedef signed      char    i8;
    typedef signed      char*   pi8;

    typedef enum boolean        _bool;
    typedef enum _null          _NULL;

#endif