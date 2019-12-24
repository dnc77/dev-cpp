/*
Date: 21 Aug 2019 14:02:34.912962067
File: weightedbinary.cpp

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

Purpose:

Version control
21 Aug 2019 Duncan Camilleri           Initial development
29 Nov 2019 Duncan Camilleri           Added accessor for ratio
29 Nov 2019 Duncan Camilleri           Fixed interchanging float and double

*/

#include <sys/time.h>
#include <random>
#include <chrono>
#include <helpers.h>
#include "weightedbinary.h"

using namespace std;

//
// Construction
//

WeightedBinary::WeightedBinary()
{
   mRandom.seed(chrono::system_clock::now().time_since_epoch().count());
   setRatio(mRatio);
   changeState();
}

WeightedBinary::~WeightedBinary()
{
}

//
// Access
//

bool WeightedBinary::getState()
{
   return mValue;
}

bool WeightedBinary::changeState()
{
   mValue = (mDistribution(mRandom) == 1 ? true : false);
   return mValue;
}

double WeightedBinary::getRatio()
{
   return mRatio;
}

void WeightedBinary::setRatio(double r)
{
   mRatio = r;

   mDistribution.reset();
   mDistribution = {1.0 - mRatio, mRatio};
   mRandom.seed(chrono::system_clock::now().time_since_epoch().count());
}
