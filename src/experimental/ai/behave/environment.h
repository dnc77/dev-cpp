/*
Date: 26 Nov 2019 11:25:17.248591902
File: environment.h

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
26 Nov 2019 Duncan Camilleri           Initial development
03 Dec 2019 Duncan Camilleri           Added ambience mood
16 Dec 2019 Duncan Camilleri           Remove residing beings
13 Jan 2020 Duncan Camilleri           Impact params

*/

#ifndef __ENVIRONMENT_H_98216C8B541FBFB5FD5CA3DC5B6355BF__
#define __ENVIRONMENT_H_98216C8B541FBFB5FD5CA3DC5B6355BF__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "environment.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "environment.h: missing include - list"
#elif not defined _GLIBCXX_STRING
#error "environment.h: missing include - string"
#elif not defined __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#error "environment.h: missing include - objref.h"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "environment.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "environment.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "environment.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "environment.h: missing include - action.h"
#endif


//
// Environment
//
// An environment is a space in BehaveWorld within which beings can interact
// with each other.
// Some examples of environments can be: a room, a football ground, a park,
// a phone call, a lift, a train etc...
// Some actions can be performed in an environment and some actions cannot.
//

class EnvironmentNode;
class Environment
{
public:
   // Construction
   Environment();
   Environment(const Environment& env);
   Environment(Environment&& env);
   virtual ~Environment();

   // Accessors
   const uint64_t& id() const;
   const char* const name() const;
   const Mood& getAmbienceMood() const;

   // Assignments
   Environment& operator=(const Environment& env);
   Environment& operator=(Environment&& env);
   Environment& operator=(const EnvironmentNode& node);

   // Data
   void reset();

   // Naming
   void name(const uint64_t id, const char* const name);

   // Alterations
   void impact(const Action& a);

protected:
   uint64_t mId;
   char mName[32];

   Mood mAmbience;

   std::list<ObjRef<const Action>> mActionRefs;       // possible actions

public:
   static intensity mCfgAmbienceImpactingMoodMinIntensity;
   static intensity mCfgAmbienceImpactingMoodMaxIntensity;
};


//
// EnvironmentNode
// A serializable environment that implements serialization functionality on top
// of Environment.
//

class EnvironmentNode : public Environment
{
public:
   // Construction
   EnvironmentNode(Node& node);
   EnvironmentNode(const Environment& env, Node& node);

   // Assignment
   EnvironmentNode& operator=(const Environment& env);

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
   bool fromPossibleActionsNode(Node& child);
};
#endif   // __ENVIRONMENT_H_98216C8B541FBFB5FD5CA3DC5B6355BF__

