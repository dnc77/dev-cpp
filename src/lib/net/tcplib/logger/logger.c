/*
Date: 22 Mar 2019 22:38:36.548196414
File: logger.c

Copyright Notice
This document is protected by the GNU General Public License v3.0.

This allows for commercial use, modification, distribution, patent and private
use of this software only when the GNU General Public License v3.0 and this
copyright notice are both attached in their original form.

For developer and author protection, the GPL clearly explains that there is no
warranty for this free software and that any source code alterations are to be
shown clearly to identify the original author as well as any subsequent changes
made and by who.

For any questions or ideas, please contact:
github:  https://github(dot)com/dnc77
email:   dnc77(at)hotmail(dot)com
web:     http://www(dot)dnc77(dot)com

Copyright (C) 2000-2019 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Provides generic log functionality.

Version control
15 Apr 2014 Duncan Camilleri           Initial development
28 Oct 2016 Duncan Camilleri           Added comments to handle members
28 Oct 2016 Duncan Camilleri           Added indentation functionality
22 Mar 2019 Duncan Camilleri           Added copyright notice
17 Oct 2020 Duncan Camilleri           Typecast missing
11 Dec 2020 Duncan Camilleri           Added logHex
12 Dec 2020 Duncan Camilleri           Added label to logHex
*/

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <net/logger.h>

//
// STRUCTS
//

// Main logger handle.
typedef struct {
   int mEnableStd;                        // print to stdout when enabled
   int mLevel;                            // at what level to start logging
   short mIndent;                         // number of indents
   char mFilename[512];                   // output file (may be empty)
   FILE* mFile;                           // file ptr when filename provided
} loghandle;

//
// FUNCTIONS
//

// Creation/destruction

// Creates a logger handle to a particular file. Each handle created must be
// destroyed with destroyLoggerHandle.
// filename:   is the file which will be logged to. All data will be lost. Can
//             be null in which case no file gets written to. For stdout, please
//             enable std.
// level:      indicates the level of the logger. Any log messages whose level
//             is equal to or less will get logged.
// std:        1 to indicate output should also go to standard output.
loghdl createLoggerHandle(const char* const filename, int level, int std)
{
   // Allocate a log handle first.
   loghandle* p = (loghandle*)malloc(sizeof(loghandle));
   if (!p) return 0;

   // Set input parameters first.
   memset(p, 0, sizeof(loghandle));
   p->mEnableStd = (std ? 1 : 0);
   p->mLevel = level;                     // Anything on or below is logged.

   // Open file.
   if (filename) {
      strncpy(p->mFilename, filename, sizeof(p->mFilename) - 1);
      p->mFile = fopen(p->mFilename, "w");
      if (!p->mFile) {
         free(p);
         return 0;
      }
   }

   // Logger opened.
   return (loghdl)p;
}

void destroyLoggerHandle(loghdl* pLogHandle)
{
   // Get the log handle.
   loghandle** p = (loghandle**)pLogHandle;
   if (!p || !*p) return;

   if (p[0]->mFile) fclose(p[0]->mFile);
   free (p[0]);
   p[0] = 0;
}

// Logging functions

// Will output indentation according the number of indents in loghdl.
// Each indent is considered to be of a specific size.
// Parameters: h     log handle
//             out   output handle
// Returns:    void
void logIndent(loghdl h, FILE* out)
{
   loghandle* p = (loghandle*)h;
   if (!p) return;

   int n = 0;
   for (; n < p->mIndent; ++n) {
      // Each indent level is two spaces.
      fprintf(out, "  ");
   }
}

// Logs data to the file and/or stdout without any form of output manipulation.
// This function does not support indentation or print any labels.
// loghdl:     is the handle which will be logged to.
// showLevel:  indicates the level of this log entry. If this level is less than
//             or equal to the logger's level it will be logged.
void justlog(loghdl h, int showLevel, const char* const fmt, ...)
{
   // Get the log handle.
   va_list va;
   loghandle* p = (loghandle*)h;
   if (!p) return;

   // Are we within the level?
   if (showLevel > p->mLevel) return;

   // Set the var argument list;
   va_start(va, fmt);

   // Just output the message.
   if (p->mEnableStd) {
      vprintf(fmt, va);
   }

   if (p->mFile) {
      vfprintf(p->mFile, fmt, va);
   }

   va_end(va);
}

void logInfo(loghdl h, int showLevel, const char* const fmt, ...)
{
   // Get the log handle.
   va_list va;
   loghandle* p = (loghandle*)h;
   if (!p) return;

   // Are we within the level?
   if (showLevel > p->mLevel) return;

   // Set the var argument list;
   va_start(va, fmt);

   // Just output the message to stdout.
   if (p->mEnableStd) {
      printf("info: ");                   // label
      logIndent(p, stdout);               // indentation
      vprintf(fmt, va);                   // message
      printf("\n");                       // new line
   }

   // Output message to file.
   if (p->mFile) {
      fprintf(p->mFile, "info: ");        // label
      logIndent(p, p->mFile);             // indentation
      vfprintf(p->mFile, fmt, va);        // message
      fprintf(p->mFile, "\n");            // new line
   }

   va_end(va);
}

void logWarn(loghdl h, int showLevel, const char* const fmt, ...)
{
   // Get the log handle.
   va_list va;
   loghandle* p = (loghandle*)h;
   if (!p) return;

   // Are we within the level?
   if (showLevel > p->mLevel) return;

   // Set the var argument list;
   va_start(va, fmt);

   // Just output the message to stdout.
   if (p->mEnableStd) {
      printf("warn: ");                   // label
      logIndent(p, stdout);               // indentation
      vprintf(fmt, va);                   // message
      printf("\n");                       // new line
   }

   // Output message to file.
   if (p->mFile) {
      fprintf(p->mFile, "warn: ");        // label
      logIndent(p, p->mFile);             // indentation
      vfprintf(p->mFile, fmt, va);        // message
      fprintf(p->mFile, "\n");            // new line
   }

   va_end(va);
}

void logErr(loghdl h, int showLevel, const char* const fmt, ...)
{
   // Get the log handle.
   va_list va;
   loghandle* p = (loghandle*)h;
   if (!p) return;

   // Are we within the level?
   if (showLevel > p->mLevel) return;

   // Set the var argument list;
   va_start(va, fmt);

   // Just output the message to stdout.
   if (p->mEnableStd) {
      printf("err:  ");                   // label
      logIndent(p, stdout);               // indentation
      vprintf(fmt, va);                   // message
      printf("\n");                       // new line
   }

   // Output message to file.
   if (p->mFile) {
      fprintf(p->mFile, "err:  ");        // label
      logIndent(p, p->mFile);             // indentation
      vfprintf(p->mFile, fmt, va);        // message
      fprintf(p->mFile, "\n");            // new line
   }

   va_end(va);
}

void logCri(loghdl h, int showLevel, const char* const fmt, ...)
{
   // Get the log handle.
   va_list va;
   loghandle* p = (loghandle*)h;
   if (!p) return;

   // Are we within the level?
   if (showLevel > p->mLevel) return;

   // Set the var argument list;
   va_start(va, fmt);

   // Just output the message to stdout.
   if (p->mEnableStd) {
      printf("!!    ");                   // label
      logIndent(p, stdout);               // indentation
      vprintf(fmt, va);                   // message
      printf(" !!\n");                    // new line
   }

   // Output message to file.
   if (p->mFile) {
      fprintf(p->mFile, "!!    ");        // label
      logIndent(p, p->mFile);             // indentation
      vfprintf(p->mFile, fmt, va);        // message
      fprintf(p->mFile, " !!\n");         // new line
   }

   va_end(va);
}

void logHex(loghdl h, int showLevel, int bytesPerRow,
            const char* const buf, const int size,
            const char* const pLabel)
{
   const unsigned char* pStart = (const unsigned char* const)buf;
   const unsigned char* const pEnd = pStart + size;
   const unsigned char* pCur = pStart;

   loghandle* p = (loghandle*)h;
   if (!p || !buf || size <= 0) return;

   // Are we within the level?
   if (showLevel > p->mLevel) return;

   // Reduce columns to nearest 8 byte.
   if (bytesPerRow < 8) bytesPerRow = 8;
   if (bytesPerRow > 8)
      bytesPerRow = bytesPerRow - (bytesPerRow % 8);

   // Print label.
   if (pLabel) {
      justlog(h, showLevel, "%s\n", pLabel);
   }

   // Print loop.
   while (pCur < pEnd) {
      int n = 0;
      int nRemaining = 0;
      char rowAddr[16];
      char bytes[(const int) bytesPerRow + 1];
      memset(rowAddr, 0, 16);
      memset(bytes, 0, bytesPerRow + 1);

      // Get memory address first...
      sprintf(rowAddr, "%016x", pCur);
      justlog(h, showLevel, "%s ", &rowAddr[11]);

      // Indentation...
      if (p->mEnableStd) {
         logIndent(p, stdout);
      }
      if (p->mFile) {
         logIndent(p, p->mFile);
      }

      // One row.
      for (; n < bytesPerRow && pCur < pEnd; ++n, ++pCur) {
         // eight byte separator
         if (n > 0 && n % 8 == 0)
            justlog(h, showLevel, " ");

         // One line ascii buffer
         bytes[n] = ((isprint(*pCur) && !isspace(*pCur)) ? *pCur : '.');

         // Output hex bytes.
         justlog(h, showLevel, "%02x ", pCur[0]);
      }

      // Output binary bytes.
      nRemaining = bytesPerRow - n;
      for (int n = 0; n < nRemaining; ++n) {
         justlog(h, showLevel, "   ");
      }

      justlog(h, showLevel, " %s\n", bytes);
   }
}

// Indentation

void logindent(loghdl h)
{
   loghandle* p = (loghandle*)h;
   if (!p) return;

   p->mIndent++;
}

void logoutdent(loghdl h)
{
   loghandle* p = (loghandle*)h;
   if (!p) return;
   if (p->mIndent == 0) return;

   p->mIndent--;
}
