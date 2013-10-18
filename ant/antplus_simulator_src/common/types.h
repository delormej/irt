/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#ifndef TYPES_H
#define TYPES_H

#include <windows.h>


//////////////////////////////////////////////////////////////////////////////////
// Public Definitions
//////////////////////////////////////////////////////////////////////////////////

#define LITTLE_ENDIAN

#define TRUE                           1
#define FALSE                          0

#if !defined(NULL)
   #define NULL                        ((void *) 0)
#endif

#define MAX_UCHAR                      0xFF
#define MAX_SCHAR                      0x7F
#define MIN_SCHAR                      0x80

#define MAX_SHORT                      0x7FFF
#define MIN_SHORT                      0x8000
#define MAX_USHORT                     0xFFFF
#define MAX_SSHORT                     0x7FFF
#define MIN_SSHORT                     0x8000

#define MAX_LONG                       0x7FFFFFFF
#define MIN_LONG                       0x80000000
#define MAX_ULONG                      0xFFFFFFFF
#define MAX_SLONG                      0x7FFFFFFF
#define MIN_SLONG                      0x80000000

#if !defined(BASETYPES)                                     // windef.h compatibility
   typedef unsigned char               BOOL;
#endif

typedef unsigned char                  UCHAR;
typedef signed char                    SCHAR;

typedef short                          SHORT;
typedef unsigned short                 USHORT;
typedef signed short                   SSHORT;

#if !defined(LONG)
   typedef long                        LONG;
#endif
typedef unsigned long                  ULONG;
typedef signed long                    SLONG;

typedef float                          FLOAT;
typedef double                         DOUBLE;

typedef union
{
   USHORT usData;
   struct
   {
      #if defined(LITTLE_ENDIAN)
         UCHAR ucLow;
         UCHAR ucHigh;
      #elif defined(BIG_ENDIAN)
         UCHAR ucHigh;
         UCHAR ucLow;
      #else
         #error
      #endif
   } stBytes;
} USHORT_UNION;

typedef union
{
   ULONG ulData;
   struct
   {
      // The least significant byte of the ULONG in this structure is
      // referenced by ucByte0.
      #if defined(LITTLE_ENDIAN)
         UCHAR ucByte0;
         UCHAR ucByte1;
         UCHAR ucByte2;
         UCHAR ucByte3;
      #elif defined(BIG_ENDIAN)
         UCHAR ucByte3;
         UCHAR ucByte2;
         UCHAR ucByte1;
         UCHAR ucByte0;
      #else
         #error
      #endif
   } stBytes;
} ULONG_UNION;
#endif // !TYPES_H

