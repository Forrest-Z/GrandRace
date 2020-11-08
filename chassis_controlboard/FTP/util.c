#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#include <ctype.h>

/**
@brief	CONVERT STRING INTO INTEGER
@return	a integer number
*/
uint16 ATOI(
    char* str,	/**< is a pointer to convert */
    uint16 base	/**< is a base value (must be in the range 2 - 16) */
)
{
    unsigned int num = 0;
    while (*str !=0)
        num = num * base + C2D(*str++);
    return num;
}

uint32 ATOI32(
    char* str,	/**< is a pointer to convert */
    uint16 base	/**< is a base value (must be in the range 2 - 16) */
)
{
    uint32 num = 0;
    while (*str !=0)
        num = num * base + C2D(*str++);
    return num;
}


/**
@brief	CONVERT STRING INTO HEX OR DECIMAL
@return	success - 1, fail - 0
*/
int ValidATOI(
    char* str, 	/**< is a pointer to string to be converted */
    int base, 	/**< is a base value (must be in the range 2 - 16) */
    int* ret		/**<  is a integer pointer to return */
)
{
    int c;
    char* tstr = str;
    if(str == 0 || *str == '\0') return 0;
    while(*tstr != '\0')
    {
        c = C2D(*tstr);
        if( c >= 0 && c < base) tstr++;
        else    return 0;
    }

    *ret = ATOI(str,base);
    return 1;
}

/**
@brief	CONVERT CHAR INTO HEX
@return	HEX

This function converts HEX(0-F) to a character
*/
char C2D(
    uint8 c	/**< is a character('0'-'F') to convert to HEX */
)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + c -'a';
    if (c >= 'A' && c <= 'F')
        return 10 + c -'A';

    return (char)c;
}


