/*
Date: 21 Aug 2019 14:02:34.912962067
File: weightedbinary.h

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

Purpose: Implements a true/false value which is determined by a randomly
         generated number based on a weight factor.

Version control
21 Aug 2019 Duncan Camilleri           Initial development
30 Sep 2019 Duncan Camilleri           Introduced discrete_distribution
29 Nov 2019 Duncan Camilleri           Added accessor for ratio
29 Nov 2019 Duncan Camilleri           Fixed interchanging float and double

*/

#ifndef __WEIGHTEDBINARY_H_B80BAFE8712B57100CB74EB5451E5437__
#define __WEIGHTEDBINARY_H_B80BAFE8712B57100CB74EB5451E5437__

// Check for missing includes.
#if not defined  (__HELPERS_H_1181F24416A281704183E457A90E8460__)
#error "weightedbinary.h: missing include - helpers.h"
#elif not defined (_GLIBCXX_RANDOM)
#error "weightedbinary.h: missing include - random"
#endif


class WeightedBinary
{
public:
   // Construction
   WeightedBinary();
   virtual ~WeightedBinary();

   // Access
   bool getState();
   bool changeState();
   double getRatio();
   void setRatio(double r);

protected:
   std::discrete_distribution<int> mDistribution;
   std::default_random_engine mRandom;

   bool mValue = false;          // value: true or false
   double mRatio = 0.5;          // probability of value being true (0.0 to 1.0)
};

#endif   // __WEIGHTEDBINARY_H_B80BAFE8712B57100CB74EB5451E5437__

