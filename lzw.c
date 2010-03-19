/***************************************************************************
*               Lempel-Ziv-Welch Encoding and Decoding Library
*
*   File    : lzw.c
*   Purpose : Use Lempel-Ziv-Welch coding to compress/decompress file
*             streams
*   Author  : Michael Dipperstein
*   Date    : January 30, 2005
*
****************************************************************************
*   UPDATES
*
*   $Id: lzw.c,v 1.4 2005/03/27 15:56:47 michael Exp $
*   $Log: lzw.c,v $
*   Revision 1.4  2005/03/27 15:56:47  michael
*   Use variable length code words.
*   Include new e-mail addres.
*
*   Revision 1.3  2005/03/10 14:26:58  michael
*   User variable names that match discription in web page.
*
*   Revision 1.2  2005/03/10 05:44:02  michael
*   Minimize the size of the dictionary.
*
*   Revision 1.1.1.1  2005/02/21 03:35:34  michael
*   Initial Release
*
****************************************************************************
*
* LZW: An ANSI C Lempel-Ziv-Welch Encoding/Decoding Routines
* Copyright (C) 2005 by Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "lzw.h"
#include "bitfile.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef struct
{
    int codeWord;               /* code word for this entry */
    unsigned char suffixChar;   /* last char in encoded string */
    unsigned int prefixCode;    /* code for remaining chars in string */
} dictionary_t;

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

#define EMPTY           -1

#define MIN_CODE_LEN    9                   /* min # bits in a code word */
#define MAX_CODE_LEN    15                  /* max # bits in a code word */

#define FIRST_CODE      (1 << CHAR_BIT)     /* value of 1st string code */

#define MAX_CODES       (1 << MAX_CODE_LEN)

#define DICT_SIZE       (MAX_CODES - FIRST_CODE)

#if (MIN_CODE_LEN <= CHAR_BIT)
#error Code words must be larger than 1 character
#endif

#if (MAX_CODE_LEN > (2 * CHAR_BIT))
#error Code words larger than 2 characters require a re-write of GetCodeWord\
 and PutCodeWord
#endif

#if ((MAX_CODES - 1) > INT_MAX)
#error There cannot be more codes than can fit in an integer
#endif

/***************************************************************************
*                                  MACROS
***************************************************************************/
#define CODE_MS_BITS(BITS)      ((BITS) - CHAR_BIT)
#define MS_BITS_MASK(BITS)      (UCHAR_MAX << (CHAR_BIT - CODE_MS_BITS(BITS)))
#define CURRENT_MAX_CODES(BITS)     (1 << (BITS))

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/

/* dictionary of string codes (encode process uses a hash key) */
dictionary_t dictionary[DICT_SIZE];
char currentCodeLen;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
int GetDictionaryIndex(int prefixCode, unsigned char c);

unsigned char DecodeRecursive(unsigned int code, FILE *fpOut);

/* file I/O */
int GetCodeWord(bit_file_t *bfpIn);
void PutCodeWord(int code, bit_file_t *bfpOut);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : LZWEncodeFile
*   Description: This routine reads an input file 1 character at a time and
*                writes out an LZW encoded version of that file.
*   Parameters : inFile - Name of file to encode
*                outFile - Name of file to write encoded output to
*   Effects    : File is encoded using the LZW algorithm with CODE_LEN
*                codes.
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int LZWEncodeFile(char *inFile, char *outFile)
{
    FILE *fpIn;                         /* uncoded input */
    bit_file_t *bfpOut;                 /* encoded output */

    int code;                           /* code for current string */
    int nextCode;                       /* next available code index */
    int c;                              /* character to add to string */
    int index;

    /* open input and output files */
    if (NULL == (fpIn = fopen(inFile, "rb")))
    {
        perror(inFile);
        return FALSE;
    }

    if (NULL == outFile)
    {
        bfpOut = MakeBitFile(stdout, BF_WRITE);
    }
    else
    {
        if (NULL == (bfpOut = BitFileOpen(outFile, BF_WRITE)))
        {
            fclose(fpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* initialize dictionary as empty */
    for (index = 0; index < DICT_SIZE; index++)
    {
        dictionary[index].codeWord = EMPTY;
    }

    /* start with 9 bit code words */
    currentCodeLen = 9;

    nextCode = FIRST_CODE;  /* code for next (first) string */

    /* now start the actual encoding process */

    code = fgetc(fpIn);     /* start with string = first character */

    while ((c = fgetc(fpIn)) != EOF)
    {
        /* look for code + c in the dictionary */
        index = GetDictionaryIndex(code, c);

        if ((dictionary[index].codeWord != EMPTY) &&
            (dictionary[index].prefixCode == code) && 
            (dictionary[index].suffixChar == c))
        {
            /* code + c is in the dictionary, make it's code the new code */
            code = dictionary[index].codeWord;
        }
        else
        {
            /* code + c is not in the dictionary, add it if there's room */
            if (nextCode < MAX_CODES)
            {
                dictionary[index].codeWord = nextCode;
                dictionary[index].prefixCode = code;
                dictionary[index].suffixChar = c;

                nextCode++;
            }

            /* are we using enough bits to write out this code word? */
            if ((code >= (CURRENT_MAX_CODES(currentCodeLen) - 1)) &&
                (currentCodeLen < MAX_CODE_LEN))
            {
                /* mark need for bigger code word with all ones */
                PutCodeWord((CURRENT_MAX_CODES(currentCodeLen) - 1), bfpOut);
                currentCodeLen++;
            }

            /* write out code for string prior to reading c */
            PutCodeWord(code, bfpOut);

            /* new code is just c */
            code = c;
        }
    }

    /* no more input.  write out last of the code. */
    PutCodeWord(code, bfpOut);

    fclose(fpIn);
    BitFileClose(bfpOut);

    return TRUE;
}

/***************************************************************************
*   Function   : GetDictionaryIndex
*   Description: This routine searches the dictionary for an entry with
*                matching prefixCode and appended character.  If one isn't
*                found, the next available index is returned.  I chose a
*                very simple hash key for dictionary look up.
*   Parameters : prefixCode - code for the string c is being appended to
*                c - character appended to string to make new string
*   Effects    : None
*   Returned   : Index for entry with matching prefix code and appended
*                character or index to empty slot if none found.
***************************************************************************/
int GetDictionaryIndex(int prefixCode, unsigned char c)
{
    int index;
    int key;

    /* compute simple hash key for code */
    key = (prefixCode << CHAR_BIT) | c;     /* concatenate c to code */
    key %= DICT_SIZE;
    index = key;

    do {
        if ((dictionary[index].prefixCode == prefixCode) &&
            (dictionary[index].suffixChar == c))
        {
            /* found prefix + c in dictionary */
            return index;
        }

        if (EMPTY == dictionary[index].codeWord)
        {
            /* found empty slot for prefix + c*/
            return index;
        }

        /* try next index */
        index ++;
        index %= DICT_SIZE;

    } while (key != index);

    return index;       /* dictionary is full and string isn't in it. */
}

/***************************************************************************
*   Function   : LZWDecodeFile
*   Description: This routine reads an input file 1 encoded string at a
*                time and decodes it using the LZW algorithm.
*   Parameters : inFile - Name of file to decode
*                outFile - Name of file to write decoded output to
*   Effects    : File is decoded using the LZW algorithm with CODE_LEN
*                codes.
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int LZWDecodeFile(char *inFile, char *outFile)
{
    bit_file_t *bfpIn;                  /* encoded input */
    FILE *fpOut;                        /* decoded output */

    unsigned int nextCode;              /* value of next code */
    unsigned int lastCode;              /* last decoded code word */
    unsigned int code;                  /* code word to decode */
    unsigned char c;                    /* last decoded character */

    /* open input and output files */
    if (NULL == (bfpIn = BitFileOpen(inFile, BF_READ)))
    {
        perror(inFile);
        return FALSE;
    }

    if (NULL == outFile)
    {
        fpOut = stdout;
    }
    else
    {
        if (NULL == (fpOut = fopen(outFile, "wb")))
        {
            BitFileClose(bfpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* start with 9 bit code words */
    currentCodeLen = 9;

    /* initialize for decoding */
    nextCode = FIRST_CODE;  /* code for next (first) string */

    /* first code from file must be a character.  use it for initial values */
    lastCode = GetCodeWord(bfpIn);
    c = lastCode;
    fputc(lastCode, fpOut);

    /* decode rest of file */
    while ((code = GetCodeWord(bfpIn)) != EOF)
    {

        /* look for code length increase marker */
        if (((CURRENT_MAX_CODES(currentCodeLen) - 1) == code) &&
            (currentCodeLen < MAX_CODE_LEN))
        {
            currentCodeLen++;
            code = GetCodeWord(bfpIn);
        }

        if (code < nextCode)
        {
            /* we have a known code.  decode it */
            c = DecodeRecursive(code, fpOut);
        }
        else
        {
            /***************************************************************
            * We got a code that's not in our dictionary.  This must be due
            * to the string + char + string + char + string exception.
            * Build the decoded string using the last character + the
            * string from the last code.
            ***************************************************************/
            unsigned char tmp;

            tmp = c;
            c = DecodeRecursive(lastCode, fpOut);
            fputc(tmp, fpOut);
        }

        /* if room, add new code to the dictionary */
        if (nextCode < MAX_CODES)
        {
            dictionary[nextCode - FIRST_CODE].prefixCode = lastCode;
            dictionary[nextCode - FIRST_CODE].suffixChar = c;
            nextCode++;
        }

        /* save character and code for use in unknown code word case */
        lastCode = code;
    }

    fclose(fpOut);
    BitFileClose(bfpIn);
    return TRUE;
}

/***************************************************************************
*   Function   : DecodeRecursive
*   Description: This function uses the dictionary to decode a code word
*                into the string it represents and write it to the output
*                file.  The string is actually built in reverse order and
*                recursion is used to write it out in the correct order.
*   Parameters : code - the code word to decode
*                fpOut - the file that the decoded code word is written to
*   Effects    : Decoded code word is written to a file
*   Returned   : The first character in the decoded string
***************************************************************************/
unsigned char DecodeRecursive(unsigned int code, FILE *fpOut)
{
    unsigned char c;
    unsigned char firstChar;

    if (code >= FIRST_CODE)
    {
        /* code word is string + c */
        c = dictionary[code - FIRST_CODE].suffixChar;
        code = dictionary[code - FIRST_CODE].prefixCode;

        /* evaluate new code word for remaining string */
        firstChar = DecodeRecursive(code, fpOut);
    }
    else
    {
        /* code word is just c */
        c = code;
        firstChar = code;
    }

    fputc(c, fpOut);
    
    return firstChar;
}

/***************************************************************************
*   Function   : GetCodeWord
*   Description: This function reads and returns a code word from an
*                encoded file.  In order to deal with endian issue the
*                code word is read least significant byte followed by the
*                remaining bits.
*   Parameters : bfpIn - bit file containing the encoded data
*   Effects    : code word is read from encoded input
*   Returned   : The next code word in the encoded file.  EOF if the end
*                of file has been reached.
*
*   NOTE: If the code word contains more than 16 bits, this routine should
*         be modified to read in all the bytes from least significant to
*         most significant followed by any left over bits.
***************************************************************************/
int GetCodeWord(bit_file_t *bfpIn)
{
    unsigned char byte;
    int code;

    /* get LS character */
    if ((code = BitFileGetChar(bfpIn)) == EOF)
    {
        return EOF;
    }


    /* get remaining bits */
    if (BitFileGetBits(bfpIn, &byte, CODE_MS_BITS(currentCodeLen)) == EOF)
    {
        return EOF;
    }

    code |= ((int)byte) << CODE_MS_BITS(currentCodeLen);

    return code;
}

/***************************************************************************
*   Function   : PutCodeWord
*   Description: This function writes a code word from to an encoded file.
*                In order to deal with endian issue the code word is
*                written least significant byte followed by the remaining
*                bits.
*   Parameters : bfpOut - bit file containing the encoded data
*                code - code word to add to the encoded data
*   Effects    : code word is written to the encoded output
*   Returned   : None
*
*   NOTE: If the code word contains more than 16 bits, this routine should
*         be modified to write out all the bytes from least significant to
*         most significant followed by any left over bits.
***************************************************************************/
void PutCodeWord(int code, bit_file_t *bfpOut)
{
    unsigned char byte;

    /* write LS character */
    byte = code & 0xFF;
    BitFilePutChar(byte, bfpOut);

    /* write remaining bits */
    byte = (code >> CODE_MS_BITS(currentCodeLen)) &
        MS_BITS_MASK(currentCodeLen);
    BitFilePutBits(bfpOut, &byte, CODE_MS_BITS(currentCodeLen));
}
