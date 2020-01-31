/*
Date: 19 Oct 2019 08:07:42.546097875
File: behavefactory.cpp

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
13 Jan 2020 Duncan Camilleri           Introduced impact parameters

*/

#include <exception>
#include <cstdint>
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#include <helpers.h>
#include <cycbuf.h>
#include "objref.h"
#include "stringlist.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "environment.h"
#include "being.h"
#include "gathering.h"
#include "relationship.h"
#include "behavefactory.h"

using namespace std;

// 
// |         |                    ,---.          |
// |---.,---.|---.,---..    ,,---.|__. ,---.,---.|--- ,---.,---.,   .
// |   ||---'|   |,---| \  / |---'|    ,---||    |    |   ||    |   |
// `---'`---'`   '`---^  `'  `---'`    `---^`---'`---'`---'`    `---|
//                                                              `---'
// 

//
// CONSTRUCTION
//
BehaveFactory::BehaveFactory()
{
}

BehaveFactory::~BehaveFactory()
{
}

//
// INSTANTIATE
//

Action* BehaveFactory::spawnAction(const char* const name, const Mood& triggers,
   const Mood& actorReactions, const Mood& recipientReactions)
{
   try {
      // Create an action and return it.
      Action a(triggers, actorReactions, recipientReactions);
      a.name(++sidAction, name);
      mActions.push_back(a);

      return &mActions.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

Being* BehaveFactory::spawnBeing(const char* const name)
{
   try {
      // Create a being and return it.
      Being b;
      b.name(++sidBeing, name);
      mBeings.push_back(b);

      return &mBeings.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

Gathering* BehaveFactory::spawnGathering(const char* const name)
{
   try {
      // Create a gathering and return it.
      Gathering g;
      g.name(++sidGathering, name);
      mGatherings.push_back(g);

      return &mGatherings.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

// Spawning a relationship requires a forging action. This action would have
// the ability to forge relationships. Two beings will be connected through
// their current emotions.
// The moods of a new relationship are set up as follows:
// MoodA pertains to BeingA's perception of the relationship.
// MoodB pertains to BeingB's perception of the relationship.
// Normally, in a new relationship, anticipation is high as it
// wanes down with time. This factor will not take play in this.
// Mood A's perception of the relationship would be a higher degree
// of Mood B's current mood compared with Mood A's current mood.
// Vice versa applies to Mood B's perception of the relationship.
// Before spawning a relationship for the factory, one should call findRel
// to determine if the relationship already exists.
// While spawning a relationship, it's not ideal to do this for efficiency
// purposes.
Relationship* BehaveFactory::spawnRelationship(const char* const name,
   const Action& forgingAction, const Being& beingA, const Being& beingB)
{
   // First ensure that the forging action has actual permission to do so.
   if (!forgingAction.forgesRelationship())
      return nullptr;

   try {
      // Calculate relationship perceptions.
      Mood calc[3];
      RelationshipData data;

      // Being A's perceptions.
      calc[0] = beingA.getCurrentMood();
      calc[1] = beingB.getCurrentMood();
      calc[2] = calc[1];
      data.mBeingRefA.set(&beingA, beingA.id());
      data.mMoodA = Mood::average(calc, 3);

      // Being B's perceptions.
      calc[0] = beingB.getCurrentMood();
      calc[1] = beingA.getCurrentMood();
      calc[2] = calc[1];
      data.mBeingRefB.set(&beingB, beingB.id());
      data.mMoodB = Mood::average(calc, 3);

      // Create a new relationship.
      Relationship r(data);
      r.name(++sidRelationship, name);

      // Finally add the relationship.
      mRelationships.push_back(r);
      return &mRelationships.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

Environment* BehaveFactory::spawnEnvironment(const char* const name)
{
   try {
      // Create an environment and return it.
      Environment env;
      env.name(++sidEnvironment, name);
      mEnvironments.push_back(env);

      return &mEnvironments.back();
   } catch (exception& e) {
      // push_back failure.
      return nullptr;
   }
}

//
// SEARCH
//

// Will find an action that has mId = id.
// If found, will return that action.
// Otherwise nullptr is returned.
Action* BehaveFactory::findAction(const uint64_t id)
{
   // Compares action's id with passed id.
   auto compare = [&](const Action& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Action>::iterator i = std::find_if(
      mActions.begin(), mActions.end(), compare);

   if (i == mActions.end()) return nullptr;

   // Found!
   return &(*i);
}

// Will find a being that has mId = id.
// If found, will return that being.
// Otherwise nullptr is returned.
Being* BehaveFactory::findBeing(const uint64_t id)
{
   // Compares being's id with passed id.
   auto compare = [&](const Being& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Being>::iterator i = std::find_if(
      mBeings.begin(), mBeings.end(), compare);

   if (i == mBeings.end()) return nullptr;

   // Found!
   return &(*i);
}

// Will find a gathering that has mId = id.
// If found, will return that gathering.
// Otherwise nullptr is returned.
Gathering* BehaveFactory::findGathering(const uint64_t id)
{
   // Compares gathering's id with passed id.
   auto compare = [&](const Gathering& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Gathering>::iterator i = std::find_if(
      mGatherings.begin(), mGatherings.end(), compare);

   if (i == mGatherings.end()) return nullptr;

   // Found!
   return &(*i);
}

// Will find a relationship that has mId = id.
// If found, will return that relationship.
// Otherwise nullptr is returned.
Relationship* BehaveFactory::findRel(const uint64_t id)
{
   // Compares relationship's id with passed id.
   auto compare = [&](const Relationship& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Relationship>::iterator i = std::find_if(
      mRelationships.begin(), mRelationships.end(), compare);

   if (i == mRelationships.end()) return nullptr;

   // Found!
   return &(*i);
}

// Will find a relationship with being a and being b.
// If found, relationship will be returned.
// Otherwise, nullptr is returned.
Relationship* BehaveFactory::findRel(
   const uint64_t beingA, const uint64_t beingB)
{
   // Compares relationship's id with passed id.
   auto compare = [&](const Relationship& iter) -> bool {
      const RelationshipData& rdList = iter.getData();

      uint64_t listBeingAId = rdList.mBeingRefA.getId();
      uint64_t listBeingBId = rdList.mBeingRefB.getId();

      if (listBeingAId == beingA && listBeingBId == beingB)
         return true;
      if (listBeingAId == beingB && listBeingBId == beingA)
         return true;

      // No match!
      return false;
   };

   // Find object.
   list<Relationship>::iterator i = std::find_if(
      mRelationships.begin(), mRelationships.end(), compare);

   if (i == mRelationships.end()) return nullptr;

   // Found!
   return &(*i);
}

// Will find an environment that has mId = id.
// If found, environment will be returned.
// Otherwise, nullptr is returned.
Environment* BehaveFactory::findEnv(const uint64_t id)
{
   // Compares environment's id with passed id.
   auto compare = [&](const Environment& iter) -> bool {
      return iter.id() == id;
   };

   // Find object.
   list<Environment>::iterator i = std::find_if(
      mEnvironments.begin(), mEnvironments.end(), compare);

   if (i == mEnvironments.end()) return nullptr;

   // Found!
   return &(*i);
}

//
// SERIALIZATION
//

// Loads a factory from the specified file.
bool BehaveFactory::load(const char* const filename)
{
   // Read xml into a Node tree as a Node map.
   Node root(mNodeData);
   if (!Node::fromFile(root, filename)) {
      return false;
   }

   // Root is always <behavefactory>.
   if (strncmp(root.getName(), "behavefactory", 13) != 0)
      return false;

   // We have a behave factory loaded. Get each child.
   list<Node>& children = root.getChildren();
   for (Node& child : children) {
      uint64_t* pnSidUpdate = nullptr;
      uint64_t nid = 0;

      // Each child node could only be one of
      // <action>, <being>, or <gathering>.
      if (strncmp(child.getName(), "action", 6) == 0) {
         // Read node - if something goes wrong, break out.
         // (Logging ideal at this level).
         if (!loadActionNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "being", 5) == 0) {
         if (!loadBeingNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "gathering", 9) == 0) {
         if (!loadGatheringNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "relationship", 12) == 0) {
         if (!loadRelationshipNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "env", 3) == 0) {
         if (!loadEnvironmentNode(child)) {
            return false;
         }
      } else if (strncmp(child.getName(), "impactparams", 12) == 0) {
         if (!loadImpactParams(child)) {
            return false;
         }
      }
   }

   // All data loaded.
   return true;
}


bool BehaveFactory::loadActionNode(Node& child)
{
   try {
      ActionNode action(child);
      if (!action.fromNode()) {
         return false;
      }

      // Add action - this should create a new action in the
      // list and not reference the current action node.
      // See operator overloading in Action.
      mActions.push_back(const_cast<const ActionNode&>(action));

      // Update sid.
      if (sidAction < action.id())
         sidAction = action.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}

bool BehaveFactory::loadBeingNode(Node& child)
{
   try {
      BeingNode being(child);
      if (!being.fromNode()) {
         return false;
      }

      // Add being - this should create a new being in the
      // list and not reference the current being node.
      // See operator overloading in Being.
      mBeings.push_back(const_cast<const BeingNode&>(being));

      // Update sid.
      if (sidBeing < being.id())
         sidBeing = being.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}
bool BehaveFactory::loadGatheringNode(Node& child)
{
   try {
      GatheringNode gathering(child);
      if (!gathering.fromNode()) {
         return false;
      }

      // Add gathering - this should create a new gathering in the
      // list and not reference the current gathering node.
      // See operator overloading in Gathering.
      mGatherings.push_back(const_cast<const GatheringNode&>(gathering));

      // Update sid.
      if (sidGathering < gathering.id())
         sidGathering = gathering.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}
bool BehaveFactory::loadRelationshipNode(Node& child)
{
   try {
      RelationshipNode relationship(child);
      if (!relationship.fromNode()) {
         return false;
      }

      // Add relationship - this should create a new relationship in the
      // list and not reference the current relationship node.
      // See operator overloading in Relationship.
      mRelationships.push_back(
         const_cast<const RelationshipNode&>(relationship)
      );

      // Update sid.
      if (sidRelationship < relationship.id())
         sidRelationship = relationship.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}
bool BehaveFactory::loadEnvironmentNode(Node& child)
{
   try {
      EnvironmentNode env(child);
      if (!env.fromNode()) {
         return false;
      }

      // Add environment - this should create a new environment in the
      // list and not reference the current environment node.
      // See operator overloading in Environment.
      mEnvironments.push_back(const_cast<const EnvironmentNode&>(env));

      // Update sid.
      if (sidEnvironment < env.id())
         sidEnvironment = env.id();
   } catch (exception& e) {
      // Catch push_back exceptions (if any).
      return false;
   }

   // Done.
   return true;
}

bool BehaveFactory::loadImpactParams(Node& ip)
{
   // We have a behave factory loaded. Get each child.
   list<Node>& children = ip.getChildren();
   for (Node& child : children) {
      if (strncmp(child.getName(), "currentmoodminintensity", 23) == 0) {
         Being::mCfgCurrentImpactingMoodMinIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "currentmoodmaxintensity", 23) == 0) {
         Being::mCfgCurrentImpactingMoodMaxIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "biasmoodminintensity", 20) == 0) {
         Being::mCfgBiasImpactingMoodMinIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "biasmoodmaxintensity", 20) == 0) {
         Being::mCfgBiasImpactingMoodMaxIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "envmoodminintensity", 19) == 0) {
         Environment::mCfgAmbienceImpactingMoodMinIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "envmoodmaxintensity", 19) == 0) {
         Environment::mCfgAmbienceImpactingMoodMaxIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "relmoodminintensity", 19) == 0) {
         Relationship::mCfgRelationshipImpactingMoodMinIntensity =
            (intensity)child.getDouble(child.getValue());
      } else if (strncmp(child.getName(), "relmoodmaxintensity", 19) == 0) {
         Relationship::mCfgRelationshipImpactingMoodMaxIntensity =
            (intensity)child.getDouble(child.getValue());
      }
   }

   return true;
}

bool BehaveFactory::save(const char* const filename)
{
   // First clear the local node data and assign to a root node.
   mNodeData.reset();
   Node root(mNodeData);

   // Set behavefactory root node.
   if (!root.setValue("behavefactory", "")) {
      return false;
   }

   // Set fail function.
   auto fail = [&]() -> bool {
      return false;
   };

   // Go through each action and serialize into the node.
   for (Action& a : mActions) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create an action node and populate.
      ActionNode an(a, *pChild);
      if (!an.toNode()) {
         return fail();
      }
   }

   // Go through each being and serialize into the node.
   for (Being& b : mBeings) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create a being node and populate.
      BeingNode bn(b, *pChild);
      if (!bn.toNode()) {
         return fail();
      }
   }

   // Go through each gathering and serialize into the node.
   for (Gathering& g : mGatherings) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create a gathering node and populate.
      GatheringNode gn(g, *pChild);
      if (!gn.toNode()) {
         return fail();
      }
   }

   // Go through each relationship and serialize into the node.
   for (Relationship& r : mRelationships) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create a gathering node and populate.
      RelationshipNode rn(r, *pChild);
      if (!rn.toNode()) {
         return fail();
      }
   }

   // Go through each environment and serialize into the node.
   for (Environment& e : mEnvironments) {
      // Spawn child node.
      Node* pChild = root.spawnChild();
      if (!pChild) {
         return fail();
      }

      // Create a environment node and populate.
      EnvironmentNode en(e, *pChild);
      if (!en.toNode()) {
         return fail();
      }
   }

   // Impact parameters.
   Node* pImpact = root.spawnChild();
   if (!pImpact || !saveImpactParams(pImpact)) {
      return fail();
   }

   // Save file.
   if (!root.toFile(filename)) {
      return fail();
   }

   // File saved.
   return true;
}

bool BehaveFactory::saveImpactParams(Node* pip)
{
   assert(pip);
   Node* pCurrMin = pip->spawnChild();
   Node* pCurrMax = pip->spawnChild();
   Node* pBiasMin = pip->spawnChild();
   Node* pBiasMax = pip->spawnChild();
   Node* pEnvMin = pip->spawnChild();
   Node* pEnvMax = pip->spawnChild();
   Node* pRelMin = pip->spawnChild();
   Node* pRelMax = pip->spawnChild();
   if (!pCurrMin || !pCurrMax || !pBiasMin || !pBiasMax ||
      !pEnvMin || !pEnvMax || !pRelMin || !pRelMax)
   {
      return false;
   }

   pCurrMin->setValue("currentmoodminintensity",
      Being::mCfgCurrentImpactingMoodMinIntensity);
   pCurrMax->setValue("currentmoodmaxintensity",
      Being::mCfgCurrentImpactingMoodMaxIntensity);
   pBiasMin->setValue("biasmoodminintensity",
      Being::mCfgBiasImpactingMoodMinIntensity);
   pBiasMax->setValue("biasmoodmaxintensity",
      Being::mCfgBiasImpactingMoodMaxIntensity);
   pEnvMin->setValue("envmoodminintensity",
      Environment::mCfgAmbienceImpactingMoodMinIntensity);
   pEnvMax->setValue("envmoodmaxintensity",
      Environment::mCfgAmbienceImpactingMoodMaxIntensity);
   pRelMin->setValue("relmoodminintensity",
      Relationship::mCfgRelationshipImpactingMoodMinIntensity);
   pRelMax->setValue("relmoodmaxintensity",
      Relationship::mCfgRelationshipImpactingMoodMaxIntensity);

   return true;
}

//
// DESTROY
//

void BehaveFactory::destroy()
{
   mEnvironments.clear();
   mRelationships.clear();
   mGatherings.clear();
   mBeings.clear();
   mActions.clear();
   sidEnvironment = 0;
   sidGathering = 0;
   sidBeing = 0;
   sidAction = 0;
}

// 
//      |        o          ,---.    |                  |o
// ,---.|---.    .,---.,---.|__.     |    ,---.,---.,---|.,---.,---.
// |   ||   |    ||    |---'|        |    |   |,---||   |||   ||   |
// `---'`---'    |`    `---'`        `---'`---'`---^`---'``   '`---|
//           `---'                                             `---'
// 

Action* findActionFrom(uint64_t id)
{
   assert(ObjRef<Action>::mpUserdata);
   BehaveFactory* pFactory = (BehaveFactory*)ObjRef<Action>::mpUserdata;

   return pFactory->findAction(id);
}

const Action* findConstActionFrom(uint64_t id)
{
   assert(ObjRef<const Action>::mpUserdata);
   BehaveFactory* pFactory = (BehaveFactory*)ObjRef<const Action>::mpUserdata;

   return const_cast<const Action*>(pFactory->findAction(id));
}

Being* findBeingFrom(uint64_t id)
{
   assert(ObjRef<Being>::mpUserdata);
   BehaveFactory* pFactory = (BehaveFactory*)ObjRef<Being>::mpUserdata;

   return pFactory->findBeing(id);
}

const Being* findConstBeingFrom(uint64_t id)
{
   assert(ObjRef<const Being>::mpUserdata);
   BehaveFactory* pFactory = (BehaveFactory*)ObjRef<const Being>::mpUserdata;

   return const_cast<const Being*>(pFactory->findBeing(id));
}

Gathering* findGatheringFrom(uint64_t id)
{
   assert(ObjRef<Gathering>::mpUserdata);
   BehaveFactory* pFactory = (BehaveFactory*)ObjRef<Gathering>::mpUserdata;

   return pFactory->findGathering(id);
}

const Gathering* findConstGatheringFrom(uint64_t id)
{
   assert(ObjRef<const Gathering>::mpUserdata);
   BehaveFactory* pFactory = (BehaveFactory*)
      ObjRef<const Gathering>::mpUserdata;

   return const_cast<const Gathering*>(pFactory->findGathering(id));
}
