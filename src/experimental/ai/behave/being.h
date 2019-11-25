/*
Date: 02 Oct 2019 15:14:15.169938276
File: being.h

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

Purpose: Represents any entity that has a mood based on Plutchik's wheel of
         emotions. The entity can be influenced by actions causing their mood
         to change. The entity can also perform a variety of actions. Such
         actions can be triggered at will or as an impulse after a change
         in their mood which is caused by another action happening to it.
         A being also has a bias mood which is their overall bias of how they
         feel. This tends to change over a long term with repeated events or
         with actions that spur sudden changes in the current mood.

Version control
02 Oct 2019 Duncan Camilleri           Initial development
06 Oct 2019 Duncan Camilleri           Removed shortname
11 Nov 2019 Duncan Camilleri           Added bias mood
24 Nov 2019 Duncan Camilleri           Doable actions converted to ObjRef
25 Nov 2019 Duncan Camilleri           Bias mood in operator= was missing

*/

#ifndef __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
#define __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "being.h: missing include - cstdint"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "mood.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "mood.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "being.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "being.h: missing include - action.h"
#elif not defined _GLIBCXX_LIST
#error "being.h: missing include - list"
#endif

//
// Being
//

class BeingNode;
class Being
{
public:
   // Construction.
   Being();
   Being(const Being& being);
   virtual ~Being();

   // Accessors
   const uint64_t& id() const       { return mId;        }

   // Assignments
   Being& operator=(const Being& being);
   Being& operator=(const BeingNode& node);

   // Data
   void reset();

   // Naming
   void name(const uint64_t id, const char* const name);

   // Mood shifting
   void forceBias(const Mood& bias);
   void forceMood(const Mood& mood);
   void impact(const Action& a);

   // Actions
   bool supportAction(const Action& a);
   

protected:
   // Name of being.
   uint64_t mId;
   char mName[32];

   // Biased, long term mood.
   Mood mBias;
   // Current mood.
   Mood mMood;
   // List of possible actions this being can perform.
   std::list<ObjRef<const Action>> mDoableActionRefs;
   // List of actions having an impact on this being.
   std::list<ActionQty> mImpactActions;
};

//
// BeingNode
// A serializable being that implements serialization functionality on top
// of Being.
//

class BeingNode : public Being
{
public:
   BeingNode(Node& node);
   BeingNode(const Being& being, Node& node);

   // Assignment
   BeingNode& operator=(const Being& being);

   // Access
   uint64_t getId()                    { return mId;     }
   Node& getNode()                     { return *mpNode; }

   // From node
   bool fromNode();

   // To node
   bool toNode();

protected:
   Node* mpNode;

   // From node privates
   bool fromDoableActionsNode(Node& node);
   bool fromImpactActionsNode(Node& node);
};


#endif   // __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
