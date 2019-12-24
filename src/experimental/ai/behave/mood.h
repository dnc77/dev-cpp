/*
Date: 30 Sep 2019 15:41:27.886071538
File: mood.h

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
12 Nov 2019 Duncan Camilleri           Purpose comment fix for trust + fear
27 Nov 2019 Duncan Camilleri           Added +operators for mood and intensity
27 Nov 2019 Duncan Camilleri           Added average()
03 Dec 2019 Duncan Camilleri           Added global intensify() function
09 Dec 2019 Duncan Camilleri           Added diminish()
24 Dec 2019 Duncan Camilleri           Major alterations to intensify & diminish

*/

#ifndef __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#define __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__

#if not defined _GLIBCXX_STRING
#error "mood.h: missing include - string"
#elif not defined _GLIBCXX_CSTDINT
#error "mood.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "mood.h: missing include - list"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "mood.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "mood.h: missing include - serializer.h"
#endif

// An intensity may vary from -1.0 to 1.0 with
// 0.0 being neutral, -1.0 being low and 1.0 being high.
using intensity = float;
static const intensity& MinIntensity = -1.0;
static const intensity& NeutralIntensity = 0.0;
static const intensity& MaxIntensity = 1.0;
static const intensity& InvalidIntensity = -2.0;
inline void embraceIntensity(intensity& i);
inline void diminishIntensity(intensity& i, const intensity by);
inline void intensifyIntensity(intensity& i, const intensity by);

typedef struct __MoodIntensities {
   intensity joy;
   intensity trust;
   intensity fear;
   intensity surprise;
   intensity sadness;
   intensity disgust;
   intensity anger;
   intensity anticipation;
} MoodIntensities;

//
// Mood
//

class MoodNode;
class Mood
{
public:
   static const unsigned short plutchikCount = 8;
   enum Plutchik : unsigned short {
      joy = 0,
      trust = 1,
      fear = 2,
      surprise = 3,
      sadness = 4,
      disgust = 5,
      anger = 6,
      anticipation = 7
   };

   static const unsigned short compositesCount = 8;
   enum PlutchikComposites : unsigned short {
      love = 0,
      submission = 1,
      awe = 2,
      disapproval = 3,
      remorse = 4,
      contempt = 5,
      aggression = 6,
      optimism = 7
   };

public:
   // Construction
   Mood();
   Mood(const Mood& mood);
   Mood(MoodIntensities& i);
   virtual ~Mood();

   // Operators
   // Mood operator+(const intensity i) const;
   Mood operator+(const Mood& b) const;
   Mood operator+=(const Mood& m);
   Mood operator-=(const Mood& m);
   Mood& operator=(intensity i);
   Mood& operator=(const Mood& mood);
   Mood& operator=(const MoodNode& moodnode);

protected:
   // Intensity levels for each of the sectors in Plutchik's wheel of emotions.
   intensity mEmotions[plutchikCount];
   intensity mComposites[compositesCount];

public:
   // Accessors
   const intensity& get(Plutchik emotion) const;

   // Intensity computations
   void diminish(Mood& target, const intensity by) const;
   void intensify(const intensity by);
   void intensify(Mood& target, const intensity by) const;
   const intensity& intensify(Plutchik emotion, const intensity by);
   void neutralize();
   void updateComposites(Plutchik emotion);
   void updateComposites();

private:
   intensity compositeCalc(Plutchik ea, Plutchik eb);

public:
   static Mood average(const Mood* const pMoods, int count);
};

//
// MoodNode
// A serializable mood that implements serialization functionality on top
// of Mood.
//

class MoodNode : public Mood
{
public:
   MoodNode(Node& node);
   MoodNode(const Mood& mood, Node& node);
   virtual ~MoodNode();

   // Assignment
   MoodNode& operator=(const Mood& mood);

   // Access
   Node& getNode()                     { return *mpNode; }

   // From node
   bool fromNode();

   // To node
   bool toNode(const char* const moodname = nullptr);

protected:
   Node* mpNode;

   // Serialization - Nodes provided should be initialized already
   static const char* const mSerPlutchik[plutchikCount];
   static const int mLenSerPlutchick[plutchikCount];
};

#endif   // __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
