/***************************************************************************
*          Header for Lempel-Ziv-Welch Encoding and Decoding Library
*
*   File    : arcode.h
*   Purpose : Provides prototypes for functions that use Lempel-Ziv-Welch
*             coding to encode/decode files.
*   Author  : Michael Dipperstein
*   Date    : January 30, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: lzw.h,v 1.1.1.1 2005/02/21 03:35:34 michael Exp $
*   $Log: lzw.h,v $
*   Revision 1.1.1.1  2005/02/21 03:35:34  michael
*   Initial Release
*
****************************************************************************
*
* LZW: An ANSI C Lempel-Ziv-Welch Encoding/Decoding Routines
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

#ifndef _LZW_H_
#define _LZW_H_

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
 /* encode inFile */
int LZWEncodeFile(char *inFile, char *outFile);

/* decode inFile*/
int LZWDecodeFile(char *inFile, char *outFile);

#endif  /* ndef _ARCODE_H_ */
