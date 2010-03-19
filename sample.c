/***************************************************************************
*                 Sample Program Using LZW Encoding Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of LZW encoding library
*   Author  : Michael Dipperstein
*   Date    : January 30, 2005
*
****************************************************************************
*   UPDATES
*
*   $Id: sample.c,v 1.2 2005/02/21 03:52:06 michael Exp $
*   $Log: sample.c,v $
*   Revision 1.2  2005/02/21 03:52:06  michael
*   Correct comments in headers.
*
*   Revision 1.1.1.1  2005/02/21 03:35:34  michael
*   Initial Release
*
****************************************************************************
*
* SAMPLE: Sample usage of Lempel-Ziv-Welch Encoding Library
* Copyright (C) 2005 by Michael Dipperstein (mdipper@cs.ucsb.edu)
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
#include <string.h>
#include "getopt.h"
#include "lzw.h"

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
char *RemovePath(char *fullPath);

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
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
****************************************************************************/
int main(int argc, char *argv[])
{
    int opt;
    char *inFile, *outFile; /* name of input & output files */
    char encode;            /* encode/decode */

    /* initialize data */
    inFile = NULL;
    outFile = NULL;
    encode = TRUE;

    /* parse command line */
    while ((opt = getopt(argc, argv, "cdi:o:h?")) != -1)
    {
        switch(opt)
        {
            case 'c':       /* compression mode */
                encode = TRUE;
                break;

            case 'd':       /* decompression mode */
                encode = FALSE;
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    free(inFile);

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    exit(EXIT_FAILURE);
                }
                else if ((inFile = (char *)malloc(strlen(optarg) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    exit(EXIT_FAILURE);
                }

                strcpy(inFile, optarg);
                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    free(outFile);

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    exit(EXIT_FAILURE);
                }
                else if ((outFile = (char *)malloc(strlen(optarg) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    exit(EXIT_FAILURE);
                }

                strcpy(outFile, optarg);
                break;

            case 'h':
            case '?':
                printf("Usage: %s <options>\n\n", RemovePath(argv[0]));
                printf("options:\n");
                printf("  -c : Encode input file to output file.\n");
                printf("  -d : Decode input file to output file.\n");
                printf("  -i <filename> : Name of input file.\n");
                printf("  -o <filename> : Name of output file.\n");
                printf("  -h | ?  : Print out command line options.\n\n");
                printf("Default: %s -c\n", RemovePath(argv[0]));
                return(EXIT_SUCCESS);
        }
    }

    /* validate command line */
    if (inFile == NULL)
    {
        fprintf(stderr, "Input file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", RemovePath(argv[0]));

        if (outFile != NULL)
        {
            free(outFile);
        }

        exit (EXIT_FAILURE);
    }
    else if (outFile == NULL)
    {
        fprintf(stderr, "Output file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", RemovePath(argv[0]));

        if (inFile != NULL)
        {
            free(inFile);
        }

        exit (EXIT_FAILURE);
    }

    /* we have valid parameters encode or decode */
    if (encode)
    {
        LZWEncodeFile(inFile, outFile);
    }
    else
    {
        LZWDecodeFile(inFile, outFile);
    }

    free(inFile);
    free(outFile);
    return EXIT_SUCCESS;
}

/****************************************************************************
*   Function   : RemovePath
*   Description: This is function accepts a pointer to the name of a file
*                along with path information and returns a pointer to the
*                character that is not part of the path.
*   Parameters : fullPath - pointer to an array of characters containing
*                           a file name and possible path modifiers.
*   Effects    : None
*   Returned   : Returns a pointer to the first character after any path
*                information.
****************************************************************************/
char *RemovePath(char *fullPath)
{
    int i;
    char *start, *tmp;                          /* start of file name */
    const char delim[3] = {'\\', '/', ':'};     /* path deliminators */

    start = fullPath;

    /* find the first character after all file path delimiters */
    for (i = 0; i < 3; i++)
    {
        tmp = strrchr(start, delim[i]);

        if (tmp != NULL)
        {
            start = tmp + 1;
        }
    }

    return start;
}
