/*
Date: 30 Sep 2019 15:41:27.886071538
File: mood.cpp

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

Purpose: Defines a mood based on Plutchik’s Wheel of Emotions.
         This is documented by Plutchik’s Wheel of Emotions - 2017 Update
         by Melissa Donaldson and was sourced from http://www.uvm.edu/~mjk/
            013%20Intro%20to%20Wildlife%20Tracking/Plutchik%27s%20Wheel%20
            of%20Emotions%20-%202017%20Update%20_%20Six%20Seconds.pdf which
         originates from Six Seconds - The emotional intelligence network.
         This explains there are eight primary sectors, each varying in
         intensity as modelled below:
         low intensity           sector         high intensity    opposite
         serenity                joy            ecstasy           sadness
         acceptance              trust          admiration        disgust
         apprehension            fear           terror            anger
         distraction             surprise       amazement         anticipation
         pensiveness             sadness        grief             joy
         boredom                 disgust        loathing          trust
         annoyance               anger          rage              fear
         interest                anticipation   vigilance         surprise

         Further to this, 8 additional emotions (composites) exist that are a
         combination of two emotions from the list above. These are as follows:
         joy + trust = love
         trust + fear = submission
         fear + surprise = awe
         surprise + sadness = disapproval
         sadness + disgust = remorse
         disgust + anger = contempt
         anger + anticipation = aggression
         anticipation + joy = optimism

Version control
30 Sep 2019 Duncan Camilleri           Initial development
03 Oct 2019 Duncan camilleri           Introduced composite calculations
06 Oct 2019 Duncan Camilleri           Added MoodIntensities
07 Oct 2019 Duncan Camilleri           Added support for string conversion
02 Nov 2019 Duncan Camilleri           Added neutralize()
04 Nov 2019 Duncan Camilleri           Added MoodNode
27 Nov 2019 Duncan Camilleri           Added +operators for mood and intensity
27 Nov 2019 Duncan Camilleri           Added average()
03 Dec 2019 Duncan Camilleri           Changed intensify() to be a multiplier
03 Dec 2019 Duncan Camilleri           Added global intensify() function
24 Dec 2019 Duncan Camilleri           Major alterations to intensify & diminish
07 Jan 2020 Duncan Camilleri           IntensifyIntensity comment fix
07 Jan 2020 Duncan Camilleri           Bug fix in intensify/diminish calcs
09 Jan 2020 Duncan Camilleri           Support flat/exp intensify & diminish
16 Jan 2020 Duncan Camilleri           scaleIntensity() should not embrace()

*/

//
// Includes
//
#include <assert.h>
#include <sys/time.h>
#include <memory.h>
#include <math.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <list>

#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include <helpers.h>

using namespace std;

//
// Intensity levels
//

// An intensity level that is too high or too low has to be adjusted for the
// entity to be able to contain it. The limits for intensities ranges from
// -1.0 to +1.0.
void embraceIntensity(intensity& i)
{
   if (i < MinIntensity) i = MinIntensity;
   if (i > MaxIntensity) i = MaxIntensity;
}

// A flat diminish simply subtracts the specified 'by' from the intensity 'i'.
// An exponential diminish subtracts a multiplication of the current value.
void diminishIntensity(intensity& i, const intensity by, bool flat /*= true*/)
{
   if (flat) {
      i -= by;
   } else {
      intensity orig = i;
      i = fabsf(i) - (i * by);

      if (orig < 0)
         i = -i;
   }

   embraceIntensity(i);
}

// A flat intensify simply adds the specified 'by' to the intensity 'i'.
// An exponential intensify adds a multiplication of the current value.
void intensifyIntensity(intensity& i, const intensity by, bool flat /*= true*/)
{
   if (flat) {
      i += by;
   } else {
      intensity orig = i;
      i = fabsf(i) + (i * by);

      if (orig < 0)
         i = -i;
   }

   embraceIntensity(i);
}

// 
// ,-.-.              |
// | | |,---.,---.,---|
// | | ||   ||   ||   |
// ` ' '`---'`---'`---'
// 

//
// Construction
//

Mood::Mood()
{
   neutralize();
}

Mood::Mood(const Mood& mood)
{
   *this = mood;
}

Mood::Mood(MoodIntensities& i)
{
   mEmotions[Plutchik::joy] = i.joy;
   mEmotions[Plutchik::trust] = i.trust;
   mEmotions[Plutchik::fear] = i.fear;
   mEmotions[Plutchik::surprise] = i.surprise;
   mEmotions[Plutchik::sadness] = i.sadness;
   mEmotions[Plutchik::disgust] = i.disgust;
   mEmotions[Plutchik::anger] = i.anger;
   mEmotions[Plutchik::anticipation] = i.anticipation;
}

Mood::~Mood()
{
}

//
// Operators
//

// Creates a new mood with this intensified by b.
Mood Mood::operator+(const Mood& b) const
{
   Mood m;
   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      m.mEmotions[n] = mEmotions[n] + b.mEmotions[n];
      embraceIntensity(m.mEmotions[n]);
   }

   // Update composites.
   m.updateComposites();
   return m;
}

// Intensifies each emotion by mood m's emotion value.
Mood Mood::operator+=(const Mood& m)
{
   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      mEmotions[n] += m.get(static_cast<Plutchik>(n));
      embraceIntensity(static_cast<intensity&>(mEmotions[n]));
   }

   // Update composites.
   updateComposites();
}

// Soften each emotion by mood m's emotion value.
Mood Mood::operator-=(const Mood& m)
{
   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      mEmotions[n] -= m.get(static_cast<Plutchik>(n));
      embraceIntensity(static_cast<intensity&>(mEmotions[n]));
   }

   // Update composites.
   updateComposites();
}

// Sets all emotions to intensity i.
Mood& Mood::operator=(intensity i)
{
   // First ensure that the intensity provided is within bounds.
   embraceIntensity(i);

   mEmotions[joy] = i;
   mEmotions[trust] = i;
   mEmotions[fear] = i;
   mEmotions[surprise] = i;
   mEmotions[sadness] = i;
   mEmotions[disgust] = i;
   mEmotions[anger] = i;
   mEmotions[anticipation] = i;
   updateComposites();

   return *this;
}

Mood& Mood::operator=(const Mood& mood)
{
   if (&mood == this)
      return *this;

   memcpy(mEmotions, mood.mEmotions, sizeof(intensity) * plutchikCount);
   memcpy(mComposites, mood.mComposites, sizeof(intensity) * compositesCount);

   return *this;
}

Mood& Mood::operator=(const MoodNode& moodnode)
{
   if (&moodnode == this)
      return *this;

   // Assign to this.
   const Mood& m = dynamic_cast<const Mood&>(moodnode);
   *this = m;

   // Return.
   return *this;
}

//
// Accessors
//

const intensity& Mood::get(Plutchik emotion) const
{
   assert(emotion < plutchikCount);
   if (emotion >= plutchikCount) return InvalidIntensity;

   return mEmotions[emotion];
}

//
// Intensity computations
//

// Will call on the diminishIntensity function on each emotion of the mood.
void Mood::diminish(Mood& target,
   const intensity by, bool flat /*= true*/) const
{
   target = *this;
   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      diminishIntensity(target.mEmotions[n], by, flat);
   }

   // Composites.
   target.updateComposites();
}

// Will call on the intensifyIntensity function on each emotion of the mood.
void Mood::intensify(const intensity by, bool flat /*= true*/)
{
   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      intensifyIntensity(mEmotions[n], by, flat);
   }

   // Update composites.
   updateComposites();
}

// This does not intensify 'this'. Instead, target is the intensified result of
// this.
void Mood::intensify(Mood& target,
   const intensity by, bool flat /*= true*/) const
{
   target = *this;

   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      intensifyIntensity(target.mEmotions[n], by, flat);
   }

   // Update composites.
   target.updateComposites();
}

// Intensifies a single emotion.
const intensity& Mood::intensify(Plutchik emotion,
   const intensity by, bool flat /*= true*/)
{
   assert(emotion < plutchikCount);
   if (emotion >= plutchikCount)
      return InvalidIntensity;

   // Intensify.
   intensifyIntensity(mEmotions[emotion], by, flat);

   // Update any composites and return.
   updateComposites(emotion);
   return mEmotions[emotion];
}

void Mood::neutralize()
{
   unsigned short n = 0;
   for (; n < plutchikCount; ++n) {
      mEmotions[n] = NeutralIntensity;
   }
   for (n = 0; n < compositesCount; ++n) {
      mComposites[n] = NeutralIntensity;
   }
}

// Calculates composite intensities related to a particular primary emotion.
void Mood::updateComposites(Plutchik emotion)
{
   switch (emotion) {
   case joy:
      mComposites[optimism] = compositeCalc(joy, anticipation);
      mComposites[love] = compositeCalc(joy, trust);
      break;
   case trust:
      mComposites[love] = compositeCalc(joy, trust);
      mComposites[submission] = compositeCalc(trust, fear);
      break;
   case fear:
      mComposites[submission] = compositeCalc(trust, fear);
      mComposites[awe] = compositeCalc(fear, surprise);
      break;
   case surprise:
      mComposites[awe] = compositeCalc(fear, surprise);
      mComposites[disapproval] = compositeCalc(surprise, sadness);
      break;
   case sadness:
      mComposites[disapproval] = compositeCalc(surprise, sadness);
      mComposites[remorse] = compositeCalc(sadness, disgust);
      break;
   case disgust:
      mComposites[remorse] = compositeCalc(sadness, disgust);
      mComposites[contempt] = compositeCalc(disgust, anger);
      break;
   case anger:
      mComposites[contempt] = compositeCalc(disgust, anger);
      mComposites[aggression] = compositeCalc(anger, anticipation);
      break;
   case anticipation:
      mComposites[aggression] = compositeCalc(anger, anticipation);
      mComposites[optimism] = compositeCalc(anticipation, joy);
      break;
   default: break;
   }
}

// Calculates composite intensities based on current values.
void Mood::updateComposites()
{
   mComposites[optimism] = compositeCalc(joy, anticipation);
   mComposites[love] = compositeCalc(joy, trust);
   mComposites[submission] = compositeCalc(trust, fear);
   mComposites[awe] = compositeCalc(fear, surprise);
   mComposites[disapproval] = compositeCalc(surprise, sadness);
   mComposites[remorse] = compositeCalc(sadness, disgust);
   mComposites[contempt] = compositeCalc(disgust, anger);
   mComposites[aggression] = compositeCalc(anger, anticipation);
   mComposites[optimism] = compositeCalc(anticipation, joy);
}

// Calculates the intensity of two moods grouped together by averaging the two
// intensities together and returning a new intensity.
intensity Mood::compositeCalc(Plutchik ea, Plutchik eb)
{
   // Shouldn't need to embrace intensity here.
   return (mEmotions[ea] + mEmotions[eb]) / 2;
}

// Determines the average mood of all moods.
Mood Mood::average(const Mood* const pMoods, int count)
{
   Mood result;

   unsigned short p = 0;
   for (; p < plutchikCount; ++p) {
      result.mEmotions[p] = NeutralIntensity;
      for (int n = 0; n < count; ++n) {
         result.mEmotions[p] += pMoods[n].mEmotions[p];
      }

      // Average and embrace.
      result.mEmotions[p] = result.mEmotions[p] / count;
      embraceIntensity(result.mEmotions[p]);
   }

   // Done!
   return result;
}

// Will change the value of intensity to a float that will fit between a new
// minimum/maximum range min/max. This is used to change the values of impacting
// intensities so that they are much smaller. This will in turn shift the moods
// by a much smaller rate than -1.0 to 1.0. Naturally, events are not meant to
// shift our moods to significant extents. But rather, this happens over time.
// Note: Impacting moods vary from a negative to a positive because they need
// to impact moods by either deducting or increasing the current mood level.
// As a result, the range of impacting moods is from
// -MaxIntensity to +MaxIntensity.
float Mood::scaleIntensity(const float min, const float max, intensity i)
{
   float oldRange = MaxIntensity - MinImpactingIntensity;
   float newRange = max - min;
   assert(oldRange > 0 && newRange > 0);

   return (((i - MinImpactingIntensity) * newRange) / oldRange) + min;
}

// 
// ,-.-.              |,   .         |
// | | |,---.,---.,---||\  |,---.,---|,---.
// | | ||   ||   ||   || \ ||   ||   ||---'
// ` ' '`---'`---'`---'`  `'`---'`---'`---'
// 

const char* const MoodNode::mSerPlutchik[plutchikCount] = {
   "joy", "trust", "fear", "surprise",
   "sadness", "disgust", "anger", "anticipation"
};
const int MoodNode::mLenSerPlutchick[plutchikCount] = {
   3, 5, 4, 7,
   7, 7, 5, 12
};

MoodNode::MoodNode(Node& node)
: mpNode(&node)
{
}

MoodNode::MoodNode(const Mood& mood, Node& node)
: mpNode(&node)
{
   *this = mood;
}

MoodNode::~MoodNode()
{
   // Don't touch child node on destruction.
}

//
// Assignment
//

MoodNode& MoodNode::operator=(const Mood& mood)
{
   Mood& target = dynamic_cast<Mood&>(*this);
   target = mood;

   return *this;
}

//
// From node
//

// Reads a node into mood.
// Format of node is as follows:
// <mood>
//    <joy>value</joy>
//    <trust>value</trust>
//    <fear>value</fear>
//    <surprise>value</surprise>
//    <sadness>value</sadness>
//    <disgust>value</disgust>
//    <anger>value</anger>
//    <anticipation>value</anticipation>
// </mood>
bool MoodNode::fromNode()
{
   if (!mpNode) return false;

   // Name of node should be 'mood'.
   const char* const pName = mpNode->getName();
   const char* const value = mpNode->getValue();
   if (!pName) return false;
   if (strncmp(pName, "mood", 4) != 0) return false;

   neutralize();

   // Process each emotion here.
   // One could opt to make this faster by not going through each
   // child node and assuming a fixed sequence but here it's
   // assumed, the user can change the ordering in a file.
   std::list<Node>& children = mpNode->getChildren();
   for (Node& child : children) {
      const char* const pName = child.getName();
      const char* const pValue = child.getValue();
      if (!pName || !pValue) continue;

      unsigned short e = 0;
      for (; e < plutchikCount; ++e) {
         if (strncmp(pName, mSerPlutchik[e], mLenSerPlutchick[e]) == 0) {
            mEmotions[e] = child.getDouble(pValue);;
            break;
         }
      }
   }

   // Once all emotions have been loaded. Update composists.
   updateComposites();
   return true;
}

// Writes to the node (resetting all existing content) to be as format
// depicted in MoodNode::fromNode().
bool MoodNode::toNode(const char* const moodname /*= nullptr*/)
{
   if (!mpNode) return false;
   mpNode->empty();

   // Set value first.
   if (!mpNode->setValue("mood", moodname)) return false;

   // Spawn all children and assign values to them.
   unsigned short e = 0;
   for (; e < plutchikCount; ++e) {
      Node* pEmotion = mpNode->spawnChild();
      if (!pEmotion) {
         mpNode->empty();
         return false;
      }

      if (!pEmotion->setValue(mSerPlutchik[e], mEmotions[e])) {
         mpNode->empty();
         return false;
      }
   }

   // Done!
   return true;
}
