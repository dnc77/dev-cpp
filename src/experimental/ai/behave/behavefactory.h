/*
Date: 19 Oct 2019 08:07:31.975760087
File: behavefactory.h

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

Purpose: Container and instantiator for all Behave related objects.

Version control
05 Oct 2019 Duncan Camilleri           Initial development
19 Oct 2019 Duncan Camilleri           Moved from bwfactory.h
02 Nov 2019 Duncan Camilleri           Introduced node
08 Nov 2019 Duncan Camilleri           Basic serialization complete
26 Nov 2019 Duncan Camilleri           Added Environments!
03 Dec 2019 Duncan Camilleri           Changed find() functions to return ptr
09 Dec 2019 Duncan Camilleri           Removed toStdout for now
11 Dec 2019 Duncan Camilleri           Introduced ObjRef loading functions
17 Dec 2019 Duncan Camilleri           Revamp of relationship spawning rules

*/

#ifndef __BEHAVEFACTORY_H_F15CB7E7A7397E6132CB1BCC3C95F0B9__
#define __BEHAVEFACTORY_H_F15CB7E7A7397E6132CB1BCC3C95F0B9__

#if not defined _GLIBCXX_CSTDINT
#error "behavefactory.h: missing include - cstdint"
#elif not defined _GLIBCXX_LIST
#error "behavefactory.h: missing include - list"
#elif not defined _GLIBCXX_STRING
#error "behavefactory.h: missing include - string"
#elif not defined __OBJREF_H_508BC7820E23F9ECDC1F73EB27A6B36E__
#error "behavefactory.h: missing include - objref.h"
#elif not defined __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#error "behavefactory.h: missing include - stringlist.h"
#elif not defined __SERIALIZER_H_B51368F9355D0A6C0E780E1F8D197E39__
#error "behavefactory.h: missing include - serializer.h"
#elif not defined __MOOD_H_AAB57A66720F9EDAC5D0668C603393AB__
#error "behavefactory.h: missing include - mood.h"
#elif not defined __ACTION_H_BA0DD2641DF3D0A449B1FA2732A6432E__
#error "behavefactory.h: missing include - action.h"
#elif not defined __ENVIRONMENT_H_98216C8B541FBFB5FD5CA3DC5B6355BF__
#error "behavefactory.h: missing include - environment.h"
#elif not defined __BEING_H_B969CD487F2BA6EDED6F3DC728739CDC__
#error "behavefactory.h: missing include - being.h"
#elif not defined __GATHERING_H_7C194EDF338C8266B57A8FB4606C6495__
#error "behavefactory.h: missing include - gathering.h"
#elif not defined __RELATIONSHIP_H_F256603D5EE1002DE62E21BDC7758768__
#error "behavefactory.h: missing include - relationship.h"
#endif

//
// Behave Factory
//

class Enact;
class BehaveFactory
{
public:
   // Construction
   BehaveFactory();
   BehaveFactory(BehaveFactory&) = delete;
   virtual ~BehaveFactory();

   // Instantiate
   Action* spawnAction(const char* const name, const Mood& triggers,
      const Mood& actorReactions, const Mood& recipientReactions);
   Being* spawnBeing(const char* const name);
   Gathering* spawnGathering(const char* const name);
   Relationship* spawnRelationship(const char* const name,
      const Action& forgingAction, const Being& beingA, const Being& beingB);
   Environment* spawnEnvironment(const char* const name);

   // Search
   Action* findAction(const uint64_t id);
   Being* findBeing(const uint64_t id);
   Gathering* findGathering(const uint64_t id);
   Relationship* findRel(const uint64_t id);
   Relationship* findRel(const uint64_t beingA, const uint64_t beingB);
   Environment* findEnv(const uint64_t id);

   // Serialization
   bool load(const char* const filename);
   bool loadActionNode(Node& child);
   bool loadBeingNode(Node& child);
   bool loadGatheringNode(Node& child);
   bool loadRelationshipNode(Node& child);
   bool loadEnvironmentNode(Node& child);
   bool save(const char* const filename);

   // Destroy
   void destroy();

private:
   // Identifiers for last object.
   uint64_t sidAction = 0;
   uint64_t sidBeing = 0;
   uint64_t sidGathering = 0;
   uint64_t sidRelationship = 0;
   uint64_t sidEnvironment = 0;

   // Node data
   StringList<slsiz::large> mNodeData;

   // Containers.
   std::list<Action> mActions;
   std::list<Being> mBeings;
   std::list<Gathering> mGatherings;
   std::list<Relationship> mRelationships;
   std::list<Environment> mEnvironments;

   friend class Enact;
};

//
// Object reference loading functions
//
// Objects in the behave factory may at times be held solely by id.
// This is to prevent storing multiple copies of the same object.
// The class ObjRef is an object reference which allows for object
// id storage. When objects are physically needed instead of just their
// id, ObjRef provides an interface to load such objects. BehaveFactory
// already loads all objects from file or through user calls. ObjRef
// will need the interface to retrieve said objects from memory. These
// functions do this.
//
Action* findActionFrom(uint64_t id);
const Action* findConstActionFrom(uint64_t id);
Being* findBeingFrom(uint64_t id);
const Being* findConstBeingFrom(uint64_t id);
Gathering* findGatheringFrom(uint64_t id);
const Gathering* findConstGatheringFrom(uint64_t id);

#endif   // __BEHAVEFACTORY_H_F15CB7E7A7397E6132CB1BCC3C95F0B9__
