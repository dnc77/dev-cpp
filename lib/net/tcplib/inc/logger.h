// Date:    15 April 2014
// Purpose: Provides generic log functionality.
//
// Version control
// Date        Author         Summary
// 15 Apr 2014 Duncan         Initial development
//

#ifndef __LOGGER_H__
#define __LOGGER_H__

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

// Indentation
void logindent(loghdl h);
void logoutdent(loghdl h);

#endif      // __LOGGER_H__

