/*
Date: 12 Dec 2019 12:04:32.567542281
File: behavehelpers.h

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

Copyright (C) 2000-2025 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: General purpose stuff that will be moved into libraries.

Version control
12 Dec 2019 Duncan Camilleri           Initial development
*/

#ifndef __BEHAVEHELPERS_H_6CD826024387A791AC955CFB37E0C960__
#define __BEHAVEHELPERS_H_6CD826024387A791AC955CFB37E0C960__

#if not defined _GLIBCXX_STRING
#error "behavehelpers.h: missing include - string.h"
#elseif not defined _WCTYPE_H
#error "behavehelpers.h: missing include - wctype.h"
#endif

//
// C STRING COMMONS
//
inline void cstrrtrim(char* s)
{
   int len = strlen(s) - 1;
   int idx = len;

   while (idx >= 0 && iswspace(s[idx])) {
      idx--;
   }
   if (idx < len) s[idx + 1] = '\0';
}

#endif   // __BEHAVEHELPERS_H_6CD826024387A791AC955CFB37E0C960__
