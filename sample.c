/***************************************************************************
*                 Sample Program Using LZW Encoding Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of LZW encoding library
*   Author  : Michael Dipperstein
*   Date    : January 30, 2005
*
****************************************************************************
*
* SAMPLE: Sample usage of Lempel-Ziv-Welch Encoding Library
* Copyright (C) 2005, 2007, 2014 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzw library.
*
* The lzw library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzw library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "optlist.h"
#include "lzw.h"

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : main
*   Description: This is the main function for this program, it validates
*                the command line input and, if valid, it will call
*                functions to encode or decode a file using the lzw
*                coding algorithm.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Encodes/Decodes input file
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int main(int argc, char *argv[])
{
    option_t *optList;
    option_t *thisOpt;
    FILE *fpIn;             /* pointer to open input file */
    FILE *fpOut;            /* pointer to open output file */
    char encode;            /* encode/decode */

    /* initialize data */
    fpIn = stdin;
    fpOut = stdout;
    encode = 1;

    /* parse command line */
    optList = GetOptList(argc, argv, "cdi:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 'c':       /* compression mode */
                encode = 1;
                break;

            case 'd':       /* decompression mode */
                encode = 0;
                break;

            case 'i':       /* input file name */
                if (fpIn != stdin)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    fclose(fpIn);

                    if (fpOut != stdout)
                    {
                        fclose(fpOut);
                    }

                    FreeOptList(optList);
                    errno = EINVAL;
                    return -1;
                }

                /* open input file as binary */
                fpIn = fopen(thisOpt->argument, "rb");
                if (fpIn == NULL)
                {
                    perror("Opening input file");

                    if (fpOut != stdout)
                    {
                        fclose(fpOut);
                    }

                    FreeOptList(optList);
                    return -1;
                }
                break;

            case 'o':       /* output file name */
                if (fpOut != stdout)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    fclose(fpOut);

                    if (fpIn != stdin)
                    {
                        fclose(fpIn);
                    }

                    FreeOptList(optList);
                    return -1;
                }

                /* open output file as binary */
                fpOut = fopen(thisOpt->argument, "wb");
                if (fpOut == NULL)
                {
                    perror("Opening output file");

                    if (fpIn != stdin)
                    {
                        fclose(fpIn);
                    }

                    FreeOptList(optList);
                    return -1;
                }
                break;

            case 'h':
            case '?':
                printf("Usage: %s <options>\n\n", FindFileName(argv[0]));
                printf("options:\n");
                printf("  -c : Encode input file to output file.\n");
                printf("  -d : Decode input file to output file.\n");
                printf("  -i <filename> : Name of input file.\n");
                printf("  -o <filename> : Name of output file.\n");
                printf("  -h | ?  : Print out command line options.\n\n");
                printf("Default: %s -c -i stdin -o stdout\n",
                    FindFileName(argv[0]));

                FreeOptList(optList);
                return 0;
        }

        optList = thisOpt->next;
        free(thisOpt);
        thisOpt = optList;
    }

    /* parsed the parameters.  now encode or decode. */
    if (encode)
    {
        LZWEncodeFile(fpIn, fpOut);
    }
    else
    {
        LZWDecodeFile(fpIn, fpOut);
    }

    free(fpIn);
    free(fpOut);
    return 0;
}
