/*
Date: 04 Oct 2019 06:47:29.251105022
File: gathering.h

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

Purpose: Defines a gathering between two or more beings.

Version control
03 Oct 2019 Duncan Camilleri           Initial development
04 Oct 2019 Duncan Camilleri           Transformed from relation.h
02 Nov 2019 Duncan Camilleri           Added copy const/assignment from being

*/

#ifndef __GATHERING_H_7C194EDF338C8266B57A8FB4606C6495__
#define __GATHERING_H_7C194EDF338C8266B57A8FB4606C6495__

#if not defined _GLIBCXX_CSTDINT
#error "gathering.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "gathering.h: missing include - list"
#elif not defined _GLIBCXX_STRING
#error "gathering.h: missing include - string"
#elif not defined __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#error "gathering.h: missing include - objref.h"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "gathering.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "gathering.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "gathering.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "gathering.h: missing include - action.h"
#elif not defined __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
#error "gathering.h: missing include - being.h"
#endif

//
// Gathering
//
// A gathering is treated as another entity in it's own right so it
// will take the properties of a being.
// A gathering has a mood which defines the emotions involved in
// as one group of beings. A gathering is between two or more seperate beings.
// Actions can also impact a gathering.
// Beings together can perform actions as one gather impacting one or more
// further entities.
//

class Gathering : public Being
{
public:
   Gathering();
   Gathering(const Gathering& gathering);
   virtual ~Gathering();

   // Accessors
   const uint64_t& id() const       { return mId;        }

   // Assignments
   Gathering& operator=(const Gathering& gathering);

   // Data
   void reset();

protected:
   std::list<ObjRef<Being>> mBeingRefs;
};

//
// GatheringNode
// A serializable gathering that implements serialization functionality on top
// of Gathering.
//

class GatheringNode : public Gathering
{
public:
   GatheringNode(Node& node);
   GatheringNode(const Gathering& gathering, Node& node);

   // Assignment
   GatheringNode& operator=(const Gathering& gathering);

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
   bool fromActableActionsNode(Node& node);
   bool fromImpactActionsNode(Node& node);
};

#endif   // __GATHERING_H_7C194EDF338C8266B57A8FB4606C6495__

