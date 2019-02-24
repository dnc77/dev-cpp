// Date:    28th January 2019
// Purpose: Various helper utilities.
//
// Version control
// 28 Jan 2019 Duncan Camilleri           Initial development
// 

#ifndef __HELPERS_H__
#define __HELPERS_H__

// Check for missing includes.
#if not defined _SYS_TIME_H
#error "helpers.h: missing include - sys/time.h"
#endif

//
// QUICKIES
//

// c++17 not installed. remove if compiling with a full c++17 support.
enum class byte : unsigned char {};

// One liners
#define max(x, y)                         (x >= y ? x : y)
#define min(x, y)                         (x <= y ? x : y)

//
// TIME
//

// Doubles the time in pt up until it reaches maxsec and maxusec.
// Subsequent calls on a timeval that is equal or greater to
// maxsec and maxusec will do nothing. When both maxsec and maxusec are 0,
// doubling will happen endlessly.
inline void doubletime(timeval& tv,
   const unsigned long maxsec = 0, const unsigned long maxusec = 0)
{
   bool endless = (maxsec == 0 && maxusec == 0);
   const long rmaxusec = (maxusec == 0) ? 1000000 : min(maxusec, 1000000);
   const long halfmaxsec = (maxsec == 0 ? 0 : maxsec / 2);
   const long halfmaxusec = (rmaxusec == 0 ? 0 : rmaxusec / 2);

   if (tv.tv_sec == 0) {
      if (tv.tv_usec == 0) {
         tv.tv_usec = 1;
      } else if (tv.tv_usec <= halfmaxusec) {
         tv.tv_usec *= 2;
      } else {
         if (maxsec > 0 || endless) {
            tv.tv_sec = 1;
            tv.tv_usec = 0;
         } else {
            tv.tv_usec = maxusec;
         }
      }
   } else if (tv.tv_sec >= halfmaxsec && !endless) {
      tv.tv_sec = maxsec;
   } else {
      if (endless || tv.tv_sec < maxsec) {
         tv.tv_sec *= 2;
      }
   }
}

#endif      // __HELPERS_H__
