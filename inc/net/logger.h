/*
Date: 22 Mar 2019 22:39:13.676720082
File: logger.h

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
22 Mar 2019 Duncan Camilleri           Added copyright notice
11 Dec 2020 Duncan Camilleri           Added logHex
12 Dec 2020 Duncan Camilleri           Added label to logHex
*/

#ifndef __LOGGER_H_35EB83B50B7DEFA4CB39D6D640C53174__
#define __LOGGER_H_35EB83B50B7DEFA4CB39D6D640C53174__

//
// STRUCTS
//
typedef void* loghdl;

// Logger levels.
// Usage of logger levels.
// A logger whose level is lognormal will display all logs that are one of
// lognormal and logminimal only. This is based on the assumption that no log is
// ever logsilent because nothing should be displayed at that level.
// I.e. A log is displayed only if the logger is at a higher or equal level.
typedef enum _logcode {
   logsilent = 0x0000,        // do not log messages at this level.
   logminimal = 0x0001,
   lognormal = 0x0002,
   logmore = 0x0003,
   logfull = 0x0004
} loglevel;

//
// FUNCTIONS
//

// Creation/destruction
loghdl createLoggerHandle(const char* const filename, int level, int std);
void destroyLoggerHandle(loghdl* pLogHandle);

// Log functions
void justlog(loghdl h, int showLevel, const char* const fmt, ...);
void logInfo(loghdl h, int showLevel, const char* const fmt, ...);
void logWarn(loghdl h, int showLevel, const char* const fmt, ...);
void logErr(loghdl h, int showLevel, const char* const fmt, ...);
void logCri(loghdl h, int showLevel, const char* const fmt, ...);
void logHex(loghdl h, int showLevel, int bytesPerRow,
   const char* const buf, const int size, const char* const pLabel);

// Indentation
void logindent(loghdl h);
void logoutdent(loghdl h);

#endif   // __LOGGER_H_35EB83B50B7DEFA4CB39D6D640C53174__
